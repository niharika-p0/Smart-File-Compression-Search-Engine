# Smart File Compression & Search Engine

**An efficient document management system combining intelligent search, data structures, document recommendations, and lossless compression.**

## Overview

The Smart File Compression & Search Engine is a C++ and Python-based project designed to make document retrieval faster and storage more efficient. It combines fundamental data structures and algorithms with intelligent search techniques to organize, search, rank, recommend, and compress text documents.

The system supports prefix-based autocomplete, exact-word search, substring search, relevance ranking, document similarity, and compression of text files.

## Key Features

- **Document Ingestion:** Load documents, extract text and metadata, and prepare content for indexing.
- **Intelligent Search:** Retrieve relevant documents using query processing and multiple search strategies.
- **Autocomplete:** Suggest words and prefixes using a Trie.
- **Search Indexing:** Use an Inverted Index for word lookup and a Suffix Array for substring search.
- **Relevance Ranking:** Rank candidate documents using BM25.
- **Document Recommendations:** Identify related documents through word-based similarity and graph traversal.
- **Lossless Compression:** Apply Burrows–Wheeler Transform (BWT), Move-to-Front (MTF), Run-Length Encoding (RLE), and Huffman Coding.
- **Searchable Compressed Storage:** Aim to reduce storage overhead while preserving access to searchable content.
- **Performance Benchmarking:** Evaluate search speed, compression ratio, indexing time, and memory usage.


## Technology Stack

| Component | Technology |
|---|---|
| Frontend | React, JavaScript, Vite, CSS |
| Backend API | Python, FastAPI |
| Search and compression engine | C++ |
| Data structures | Trie, Inverted Index, Suffix Array, LRU Cache |
| Search ranking | BM25 |
| Compression algorithms | BWT, MTF, RLE, Huffman Coding |
| Development tools | Git, GitHub, npm, Python virtual environment |

## How It Works

1. **Ingestion:** Documents are loaded, their text is extracted, and the content is normalized.
2. **Indexing:** Search structures are created to support prefix, exact-word, and substring queries.
3. **Query processing:** The system classifies a query and retrieves matching candidate documents.
4. **Ranking:** BM25 assigns relevance scores to candidate documents.
5. **Recommendations:** Similarity calculations and graph traversal identify related documents.
6. **Compression:** The compression pipeline transforms the input and applies lossless coding.
7. **Evaluation:** Search, indexing, and compression performance are measured.

## Data Structures & Algorithms

| Algorithm / Structure | Purpose |
|---|---|
| Trie | Prefix matching and autocomplete |
| Inverted Index | Efficient exact-word lookup |
| Suffix Array | Substring and pattern search |
| LRU Cache | Reuse recently accessed query results |
| BM25 | Relevance-based document ranking |
| Document Graph | Represent document relationships |
| Jaccard Similarity | Measure word-set similarity |
| Breadth-First Search (BFS) | Traverse document relationships |
| BWT + MTF + RLE + Huffman | Lossless compression pipeline |

## Installation & Execution

### Prerequisites

- Node.js and npm
- Python
- A C++ compiler compatible with the project's build configuration

### 1. Clone the repository

```bash
git clone <YOUR_GITHUB_REPOSITORY_URL>
cd SmartFileSearchEngine
```

Replace the placeholder with your actual GitHub repository URL.

### 2. Start the backend

On Windows PowerShell:

```powershell
.\backend\venv\Scripts\python.exe -m uvicorn backend.app.main:app --host 127.0.0.1 --port 8000
```

If the virtual environment is not present, create one and install the backend dependencies listed in `backend/requirements.txt`.

### 3. Start the frontend

Open a second terminal:

```powershell
cd frontend
npm install
npm run dev
```

### 4. Open the application

- Frontend: `http://localhost:5173`
- API documentation (Swagger UI): `http://127.0.0.1:8000/docs`

Ensure the C++ engine executable is available at the path expected by the backend.

## Performance Evaluation

The project can be evaluated using:

- Search response time
- Autocomplete response time
- Index construction time
- Compression and restoration time
- Compression ratio
- Index storage size
- Memory usage

Use representative text documents and report measured results rather than assuming a particular performance improvement.

## Project Goals

- Demonstrate practical applications of data structures and algorithms.
- Improve document retrieval through indexing and relevance ranking.
- Explore lossless compression techniques for text storage.
- Combine search, compression, and document recommendations in one application.
- Measure algorithm performance using reproducible benchmarks.

## Future Enhancements

- Support for additional document formats.
- Improved spelling correction and query suggestions.
- Incremental indexing when documents are added or modified.
- More advanced document similarity and recommendation techniques.
- Search directly over compressed representations where supported by the index design.

## License

Add your preferred open-source license before distributing the project publicly.

---

*Built as a data structures and algorithms project exploring intelligent document retrieval and efficient text storage.*
