
import json
import shutil
import uuid
from pathlib import Path
import hashlib

import uvicorn
from fastapi import FastAPI, File, HTTPException, UploadFile
from fastapi.middleware.cors import CORSMiddleware
from backend.app.services.cpp_engine import autocomplete
from fastapi.responses import FileResponse
from backend.app.services.cpp_engine import compress_file, restore_file

from backend.app.services.document_parser import (
    SUPPORTED_EXTENSIONS,
    extract_text,
    get_file_metadata,
)

from backend.app.services.cpp_engine import (
    search_documents,
    autocomplete as cpp_autocomplete,
    engine_status as cpp_engine_status,
    recommend_documents,
)


APP_DIR = Path(__file__).resolve().parent
BACKEND_DIR = APP_DIR.parent
PROJECT_DIR = BACKEND_DIR.parent

UPLOAD_DIR = BACKEND_DIR / "uploads"
EXTRACTED_DIR = BACKEND_DIR / "extracted"
DOCUMENT_DIR = PROJECT_DIR / "data" / "documents"
COMPRESSED_DIR = PROJECT_DIR / "data" / "compressed"

for directory in (
    UPLOAD_DIR,
    EXTRACTED_DIR,
    DOCUMENT_DIR,
    COMPRESSED_DIR,
):
    directory.mkdir(parents=True, exist_ok=True)

app = FastAPI(
    title="Smart File Compression & Search Engine API",
    version="1.0.0",
)

app.add_middleware(
    CORSMiddleware,
    allow_origins=[
        origin.strip()
        for origin in (
            "http://localhost:5173,http://127.0.0.1:5173,"
            + __import__("os").environ.get("FRONTEND_URL", "")
        ).split(",")
        if origin.strip()
    ],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


def read_document(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="ignore")


def get_document_records():
    records = []

    for path in sorted(DOCUMENT_DIR.glob("*.txt")):
        try:
            content = read_document(path)
            records.append({
                "document_id": path.stem,
                "filename": path.name,
                "original_filename": path.name,
                "stored_filename": path.name,
                "extension": path.suffix.lower(),
                "size": path.stat().st_size,
                "word_count": len(content.split()),
                "character_count": len(content),
                "searchable": True,
            })
        except OSError:
            continue

    return records


def normalize_search_response(result, query: str):
    if isinstance(result, list):
        return {
            "success": True,
            "query": query,
            "result_count": len(result),
            "results": result,
        }

    if not isinstance(result, dict):
        return {
            "success": False,
            "query": query,
            "result_count": 0,
            "results": [],
            "error": "Unexpected response from C++ engine",
        }

    if result.get("success") is False:
        return {
            "success": False,
            "query": query,
            "result_count": 0,
            "results": [],
            "error": result.get("error", "C++ search failed"),
        }

    raw_results = result.get("results", [])
    if not isinstance(raw_results, list):
        raw_results = []

    normalized_results = []

    for item in raw_results:
        if isinstance(item, str):
            normalized_results.append({
                "file_name": item,
                "score": 0,
            })
        elif isinstance(item, dict):
            normalized_results.append({
                **item,
                "file_name": (
                    item.get("file_name")
                    or item.get("filename")
                    or item.get("fileName")
                    or "Unknown document"
                ),
                "score": item.get("score", 0),
            })

    return {
        **result,
        "success": result.get("success", True),
        "query": result.get("query", query),
        "result_count": len(normalized_results),
        "results": normalized_results,
    }


def extract_suggestion_words(result):
    if isinstance(result, list):
        raw_items = result
    elif isinstance(result, dict):
        raw_items = (
            result.get("suggestions")
            or result.get("results")
            or result.get("words")
            or result.get("matches")
            or []
        )
    else:
        raw_items = []

    suggestions = []

    if not isinstance(raw_items, list):
        return suggestions

    for item in raw_items:
        if isinstance(item, str):
            word = item
        elif isinstance(item, dict):
            word = (
                item.get("word")
                or item.get("suggestion")
                or item.get("text")
                or item.get("term")
                or item.get("value")
                or ""
            )
        else:
            continue

        if word and word not in suggestions:
            suggestions.append(word)

    return suggestions


@app.get("/")
def root():
    return {
        "message": "Smart File Compression & Search Engine API",
        "status": "running",
    }


@app.get("/health")
def health():
    return {"status": "healthy"}


@app.get("/project")
def project_info():
    return {
        "name": "Smart File Compression & Search Engine",
        "backend": "FastAPI",
        "search_engine": "C++",
        "features": [
            "BM25 ranked search",
            "Trie autocomplete",
            "Inverted index",
            "Suffix array substring search",
            "Document upload and text extraction",
            "Smart compression through the C++ engine",
            "Document recommendations",
            "Performance analytics",
        ],
    }


@app.get("/api/info")
def api_info():
    return project_info()


@app.get("/documents")
def list_documents():
    records = get_document_records()
    return {
        "success": True,
        "document_count": len(records),
        "documents": records,
    }



@app.get("/engine/status")
def engine_status():
    records = get_document_records()

    bridge_path = PROJECT_DIR / "WebBridge.exe"
    document_directory_exists = DOCUMENT_DIR.is_dir()

    cpp_status = cpp_engine_status(str(DOCUMENT_DIR))

    engine_ready = (
        bridge_path.is_file()
        and document_directory_exists
        and isinstance(cpp_status, dict)
        and cpp_status.get("success") is True
        and cpp_status.get("status") == "ready"
    )

    return {
        "status": "ready" if engine_ready else "disconnected",
        "engine": "C++",
        "document_count": len(records),
        "document_directory": str(DOCUMENT_DIR),
        "web_bridge_exists": bridge_path.is_file(),
        "document_directory_exists": document_directory_exists,
        "cpp_status": cpp_status,
    }


@app.get("/dashboard/stats")
def dashboard_stats():
    records = get_document_records()

    total_words = sum(
        record["word_count"] for record in records
    )
    total_characters = sum(
        record["character_count"] for record in records
    )
    total_bytes = sum(
        record["size"] for record in records
    )

    return {
        "success": True,
        "document_count": len(records),
        "total_words": total_words,
        "total_characters": total_characters,
        "total_bytes": total_bytes,
        "library_size_bytes": total_bytes,
        "engine": engine_status(),
        "recent_documents": records[-5:][::-1],
    }


@app.get("/documents/{filename}")
def get_document(filename: str):
    if Path(filename).name != filename:
        raise HTTPException(
            status_code=400,
            detail="Invalid filename",
        )

    file_path = DOCUMENT_DIR / filename

    if not file_path.is_file() or file_path.suffix.lower() != ".txt":
        raise HTTPException(
            status_code=404,
            detail="Document not found",
        )

    content = read_document(file_path)

    return {
        "filename": filename,
        "word_count": len(content.split()),
        "character_count": len(content),
        "content": content,
    }


@app.post("/upload")
async def upload_document(file: UploadFile = File(...)):
    original_name = Path(file.filename or "").name

    if not original_name:
        raise HTTPException(
            status_code=400,
            detail="A filename is required",
        )

    extension = Path(original_name).suffix.lower()

    if extension not in SUPPORTED_EXTENSIONS:
        raise HTTPException(
            status_code=400,
            detail=(
                "Unsupported file type. Supported formats: "
                + ", ".join(sorted(SUPPORTED_EXTENSIONS))
            ),
        )

    unique_id = str(uuid.uuid4())
    uploaded_path = UPLOAD_DIR / f"{unique_id}{extension}"
    extracted_path = EXTRACTED_DIR / f"{unique_id}.txt"
    searchable_path = DOCUMENT_DIR / f"{unique_id}.txt"

    try:
        with uploaded_path.open("wb") as output:
            shutil.copyfileobj(file.file, output)

        text = extract_text(uploaded_path)

        if not text or not text.strip():
            uploaded_path.unlink(missing_ok=True)
            raise HTTPException(
                status_code=422,
                detail=(
                    "No text could be extracted. "
                    "For scanned PDFs, check that OCR is installed "
                    "and the PDF can be converted to images."
                ),
            )

        extracted_path.write_text(
            text,
            encoding="utf-8",
        )
        searchable_path.write_text(
            text,
            encoding="utf-8",
        )

        metadata = get_file_metadata(uploaded_path, text)

        return {
            "success": True,
            "document_id": unique_id,
            "filename": original_name,
            "original_filename": original_name,
            "extension": extension,
            "size": metadata["size"],
            "word_count": metadata["word_count"],
            "character_count": metadata["character_count"],
            "text_extracted": True,
            "searchable": True,
            "uploaded_file": str(uploaded_path),
            "extracted_file": str(extracted_path),
            "cpp_search_file": str(searchable_path),
        }

    except HTTPException:
        raise

    except Exception as error:
        extracted_path.unlink(missing_ok=True)
        searchable_path.unlink(missing_ok=True)

        raise HTTPException(
            status_code=500,
            detail=f"Document processing failed: {error}",
        ) from error

    finally:
        await file.close()


@app.get("/search")
def search(q: str = ""):
    query = q.strip()

    if not query:
        return {
            "success": True,
            "query": "",
            "result_count": 0,
            "results": [],
        }

    result = search_documents(
        query,
        str(DOCUMENT_DIR),
    )

    response = normalize_search_response(result, query)

    if not response["success"]:
        raise HTTPException(
            status_code=502,
            detail=response.get(
                "error",
                "C++ search engine failed",
            ),
        )

    return response


@app.get("/autocomplete")
def autocomplete_suggestions(
    q: str = "",
    limit: int = 5,
):
    prefix = q.strip()
    limit = max(1, min(limit, 10))

    if not prefix:
        return {
            "success": True,
            "query": "",
            "suggestions": [],
        }

    result = cpp_autocomplete(
        prefix,
        str(DOCUMENT_DIR),
    )

    if not isinstance(result, (dict, list)):
        raise HTTPException(
            status_code=502,
            detail="Unexpected autocomplete response from C++ engine",
        )

    if isinstance(result, dict) and result.get("success") is False:
        raise HTTPException(
            status_code=502,
            detail=result.get("error", "Autocomplete failed"),
        )

    suggestions = extract_suggestion_words(result)

    return {
        "success": True,
        "query": prefix,
        "suggestions": suggestions[:limit],
    }
    


if __name__ == "__main__":
    uvicorn.run(
        "app.main:app",
        host="127.0.0.1",
        port=8000,
        reload=True,
    )
    

@app.post("/compress")
def compress_document(filename: str):
    source = (DOCUMENT_DIR / filename).resolve()

    if source.parent != DOCUMENT_DIR.resolve() or not source.is_file():
        raise HTTPException(status_code=404, detail="Document not found")

    output = (COMPRESSED_DIR / f"{source.name}.huff").resolve()

    result = compress_file(str(source), str(output))

    if not result.get("success"):
        raise HTTPException(
            status_code=500,
            detail=result.get("error", "Compression failed"),
        )
    return {
        **result,
        "compressed_filename": output.name,
    }    
        

@app.get("/recommendations/{document_id}")
def get_recommendations(document_id: int, limit: int = 5):
    result = recommend_documents(
        document_id=document_id,
        document_directory=str(DOCUMENT_DIR),
        limit=limit,
    )

    if not isinstance(result, dict):
        raise HTTPException(
            status_code=502,
            detail="Unexpected response from recommendation engine.",
        )

    if result.get("success") is False:
        raise HTTPException(
            status_code=502,
            detail=result.get("error", "Recommendation failed."),
        )

    return result
    



@app.post("/restore")
def restore_document(filename: str):
    source = (COMPRESSED_DIR / filename).resolve()

    # Validate the compressed file.
    if (
        source.parent != COMPRESSED_DIR.resolve()
        or not source.is_file()
        or source.suffix.lower() != ".huff"
    ):
        raise HTTPException(
            status_code=404,
            detail="Compressed file not found",
        )

    # Generate the restored filename.
    restored_name = source.name.removesuffix(".huff")
    output = (
        EXTRACTED_DIR / f"restored_{restored_name}"
    ).resolve()

    # Ensure the output stays inside the extracted directory.
    if output.parent != EXTRACTED_DIR.resolve():
        raise HTTPException(
            status_code=400,
            detail="Invalid restored filename",
        )

    # Restore using the C++ engine.
    result = restore_file(
        str(source),
        str(output),
    )

    if not result.get("success"):
        raise HTTPException(
            status_code=500,
            detail=result.get(
                "error",
                "Restoration failed",
            ),
        )

    # Find the corresponding original document.
    original = (
        DOCUMENT_DIR / restored_name
    ).resolve()

    integrity_verified = None
    original_sha256 = None
    restored_sha256 = None

    # Compare hashes of the actual file bytes.
    if (
        original.parent == DOCUMENT_DIR.resolve()
        and original.is_file()
    ):
        original_sha256 = hashlib.sha256(
            original.read_bytes()
        ).hexdigest()

        restored_sha256 = hashlib.sha256(
            output.read_bytes()
        ).hexdigest()

        integrity_verified = (
            original_sha256 == restored_sha256
        )

    # Return the restoration result.
    return {
        **result,
        "restored_filename": output.name,
        "restored_size": output.stat().st_size,
        "integrity_verified": integrity_verified,
        "original_sha256": original_sha256,
        "restored_sha256": restored_sha256,
        "integrity_message": (
            "Restored file matches the original byte-for-byte."
            if integrity_verified is True
            else (
                "Restored file differs from the original."
                if integrity_verified is False
                else (
                    "Original document was not found; "
                    "integrity could not be verified."
                )
            )
        ),
    }

   

@app.get("/restored-files/{filename}")
def download_restored_file(filename: str):
    file_path = (EXTRACTED_DIR / filename).resolve()

    if file_path.parent != EXTRACTED_DIR.resolve() or not file_path.is_file():
        raise HTTPException(
            status_code=404,
            detail="Restored file not found",
        )

    return FileResponse(
        path=str(file_path),
        filename=file_path.name,
        media_type="application/octet-stream",
    )