
import { useCallback, useEffect, useMemo, useState } from "react";
import axios from "axios";
import "./App.css";

const API_URL = (import.meta.env.VITE_API_URL || "http://127.0.0.1:8000").replace(/\/$/, "");

const NAV_ITEMS = [
  { id: "dashboard", label: "Dashboard", icon: "▦" },
  { id: "search", label: "Smart Search", icon: "⌕" },
  { id: "documents", label: "Documents", icon: "▤" },
  { id: "compression", label: "Compression", icon: "▣" },
  { id: "explore", label: "Explore", icon: "⌘" },
  { id: "analytics", label: "Analytics", icon: "↗" },
  { id: "settings", label: "Settings", icon: "⚙" },
];

const PAGE_DETAILS = {
  analytics: {
    eyebrow: "PERFORMANCE INSIGHTS",
    title: "Analytics",
    description: "Understand your document collection and storage.",
    icon: "↗",
    features: [
      "Indexed document count",
      "Total indexed words",
      "Document library size",
      "Compression performance",
    ],
  },
  settings: {
    eyebrow: "WORKSPACE PREFERENCES",
    title: "Settings",
    description: "Manage your SmartFile workspace.",
    icon: "⚙",
    features: [
      "React frontend",
      "FastAPI backend",
      "C++ search engine",
      "Local document storage",
    ],
  },
};

function formatBytes(bytes = 0) {
  const value = Number(bytes) || 0;

  if (value < 1024) return `${value} B`;
  if (value < 1024 * 1024) return `${(value / 1024).toFixed(2)} KB`;

  return `${(value / (1024 * 1024)).toFixed(2)} MB`;
}

function getFilename(item) {
  return (
    item?.stored_filename ||
    item?.filename ||
    item?.file_name ||
    item?.fileName ||
    item?.original_filename ||
    "Unknown document"
  );
}

function getErrorMessage(error) {
  return (
    error?.response?.data?.detail ||
    error?.response?.data?.error ||
    error?.message ||
    "Something went wrong."
  );
}

export default function App() {
  const [activePage, setActivePage] = useState("dashboard");
  const [isDark, setIsDark] = useState(true);

  const [documents, setDocuments] = useState([]);
  const [documentCount, setDocumentCount] = useState(0);
  const [dashboardStats, setDashboardStats] = useState({});
  const [engineConnected, setEngineConnected] = useState(false);

  const [loading, setLoading] = useState(false);
  const [pageError, setPageError] = useState("");

  const [searchQuery, setSearchQuery] = useState("");
  const [searchResults, setSearchResults] = useState([]);
  const [suggestions, setSuggestions] = useState([]);
  const [searchLoading, setSearchLoading] = useState(false);

  const [uploadBusy, setUploadBusy] = useState(false);
  const [uploadMessage, setUploadMessage] = useState("");

  const [previewDocument, setPreviewDocument] = useState(null);
  const [previewLoading, setPreviewLoading] = useState(false);

  const [compressionFilename, setCompressionFilename] = useState("");
  const [compressionBusy, setCompressionBusy] = useState(false);
  const [compressionResult, setCompressionResult] = useState(null);
  const [compressionError, setCompressionError] = useState("");

  const [restoreFilename, setRestoreFilename] = useState("");
const [restoreBusy, setRestoreBusy] = useState(false);
const [restoreError, setRestoreError] = useState("");
const [restoreMessage, setRestoreMessage] = useState("");
const [downloadBusy, setDownloadBusy] = useState(false);
const [restoreResult, setRestoreResult] = useState(null);

  const [recommendationDocumentId, setRecommendationDocumentId] =
    useState("4");
  const [recommendations, setRecommendations] = useState([]);
  const [recommendationSource, setRecommendationSource] = useState(null);
  const [recommendationsLoading, setRecommendationsLoading] = useState(false);
  const [recommendationsError, setRecommendationsError] = useState("");

  const [compressedFiles, setCompressedFiles] = useState([]);

  const fetchDocuments = useCallback(async () => {
    const response = await axios.get(`${API_URL}/documents`);
    const list = response.data?.documents || [];

    setDocuments(list);
    setDocumentCount(response.data?.document_count ?? list.length);

    return list;
  }, []);

  const fetchDashboard = useCallback(async () => {
    const response = await axios.get(`${API_URL}/dashboard/stats`);
    setDashboardStats(response.data || {});
  }, []);

  const fetchEngineStatus = useCallback(async () => {
    try {
      const response = await axios.get(`${API_URL}/engine/status`);

      setEngineConnected(
        response.data?.status === "ready" ||
          response.data?.cpp_status?.status === "ready"
      );
    } catch {
      setEngineConnected(false);
    }
  }, []);

  const refreshWorkspace = useCallback(async () => {
    setLoading(true);
    setPageError("");

    try {
      await Promise.all([
        fetchDocuments(),
        fetchDashboard(),
        fetchEngineStatus(),
      ]);
    } catch (error) {
      setPageError(getErrorMessage(error));
    } finally {
      setLoading(false);
    }
  }, [fetchDocuments, fetchDashboard, fetchEngineStatus]);

  useEffect(() => {
    refreshWorkspace();
  }, [refreshWorkspace]);

  const goToPage = (page) => {
    setActivePage(page);
    setPageError("");
  };

  const totalWords = useMemo(() => {
    if (dashboardStats.total_words != null) {
      return Number(dashboardStats.total_words) || 0;
    }

    return documents.reduce(
      (sum, item) => sum + (Number(item.word_count) || 0),
      0
    );
  }, [dashboardStats, documents]);

  const totalBytes = useMemo(() => {
    if (dashboardStats.total_bytes != null) {
      return Number(dashboardStats.total_bytes) || 0;
    }

    return documents.reduce(
      (sum, item) => sum + (Number(item.size) || 0),
      0
    );
  }, [dashboardStats, documents]);

  const openDocumentPreview = async (item) => {
    const filename =
      typeof item === "string" ? item : getFilename(item);

    setPreviewDocument({ filename, content: "" });
    setPreviewLoading(true);

    try {
      const response = await axios.get(
        `${API_URL}/documents/${encodeURIComponent(filename)}`
      );

      setPreviewDocument({
        filename: response.data?.filename || filename,
        content: response.data?.content || "",
        wordCount: response.data?.word_count ?? 0,
        characterCount: response.data?.character_count ?? 0,
      });
    } catch (error) {
      setPreviewDocument({
        filename,
        content: `Could not load document preview.\n\n${getErrorMessage(error)}`,
      });
    } finally {
      setPreviewLoading(false);
    }
  };

  const runSearch = async (query = searchQuery) => {
    const cleanQuery = query.trim();

    if (!cleanQuery) {
      setSearchResults([]);
      setSuggestions([]);
      return;
    }

    setSearchQuery(cleanQuery);
    setSearchLoading(true);
    setPageError("");
    setSuggestions([]);

    try {
      const response = await axios.get(`${API_URL}/search`, {
        params: { q: cleanQuery },
      });

      setSearchResults(response.data?.results || []);
    } catch (error) {
      setSearchResults([]);
      setPageError(`Search failed: ${getErrorMessage(error)}`);
    } finally {
      setSearchLoading(false);
    }
  };

  useEffect(() => {
    if (activePage !== "search") return undefined;

    const prefix = searchQuery.trim();

    if (!prefix) {
      setSuggestions([]);
      return undefined;
    }

    let cancelled = false;

    const timer = setTimeout(async () => {
      try {
        const response = await axios.get(`${API_URL}/autocomplete`, {
          params: { q: prefix, limit: 5 },
        });

        if (!cancelled) {
          setSuggestions(response.data?.suggestions || []);
        }
      } catch {
        if (!cancelled) setSuggestions([]);
      }
    }, 250);

    return () => {
      cancelled = true;
      clearTimeout(timer);
    };
  }, [searchQuery, activePage]);

  const handleUpload = async (event) => {
    const files = Array.from(event.target.files || []);
    if (!files.length) return;

    setUploadBusy(true);
    setUploadMessage("");
    setPageError("");

    try {
      for (const file of files) {
        const formData = new FormData();
        formData.append("file", file);
        await axios.post(`${API_URL}/upload`, formData);
      }

      setUploadMessage(
        `${files.length} document${files.length === 1 ? "" : "s"} uploaded successfully.`
      );

      await refreshWorkspace();
    } catch (error) {
      setUploadMessage(`Upload failed: ${getErrorMessage(error)}`);
    } finally {
      setUploadBusy(false);
      event.target.value = "";
    }
  };

  const handleCompress = async () => {
    if (!compressionFilename) {
      setCompressionError("Choose a document to compress.");
      return;
    }

    setCompressionBusy(true);
    setCompressionError("");
    setCompressionResult(null);

    try {
      const response = await axios.post(`${API_URL}/compress`, null, {
        params: { filename: compressionFilename },
      });

      setCompressionResult(response.data);
      console.log("Compression API response:", response.data);

      const compressedName =
  response.data?.compressed_filename ||
  response.data?.filename ||
  response.data?.output_filename;

if (compressedName) {
  setRestoreFilename(compressedName);
}

      await refreshWorkspace();
    } catch (error) {
      setCompressionError(getErrorMessage(error));
    } finally {
      setCompressionBusy(false);
    }
  };

  
const handleRestore = async () => {
  const filename = restoreFilename.trim();

  if (!filename) {
    setRestoreError("Enter a .huff filename to restore.");
    setRestoreMessage("");
    return;
  }

  setRestoreBusy(true);
  setRestoreError("");
  setRestoreMessage("");
  setRestoreResult(null);

  try {
    const response = await axios.post(
      `${API_URL}/restore`,
      null,
      {
        params: { filename },
        timeout: 130000,
      }
    );

    setRestoreResult(response.data);
    setRestoreMessage(
      response.data?.integrity_verified === true
        ? "Document restored successfully. Integrity verified!"
        : "Document restored, but integrity could not be verified."
    );
  } catch (error) {
    setRestoreError(getErrorMessage(error));
  } finally {
    setRestoreBusy(false);
  }
};




  const handleDownloadRestored = async () => {
    const filename = restoreResult?.restored_filename;

    if (!filename) {
      setCompressionError("Restore a document before downloading.");
      return;
    }

    setDownloadBusy(true);
    setCompressionError("");

    try {
      const response = await axios.get(
        `${API_URL}/restored-files/${encodeURIComponent(filename)}`,
        { responseType: "blob" }
      );

      const url = URL.createObjectURL(response.data);
      const link = document.createElement("a");

      link.href = url;
      link.download = filename;
      document.body.appendChild(link);
      link.click();
      link.remove();

      setTimeout(() => URL.revokeObjectURL(url), 1000);
    } catch (error) {
      setCompressionError(getErrorMessage(error));
    } finally {
      setDownloadBusy(false);
    }
  };

  const handleGetRecommendations = async () => {
    if (recommendationDocumentId === "") {
      setRecommendationsError("Select a document ID.");
      return;
    }

    setRecommendationsLoading(true);
    setRecommendationsError("");
    setRecommendations([]);
    setRecommendationSource(null);

    try {
      const response = await axios.get(
        `${API_URL}/recommendations/${encodeURIComponent(recommendationDocumentId)}`,
        { params: { limit: 5 } }
      );

      const data = response.data || {};
      setRecommendations(data.recommendations || []);
      setRecommendationSource(data);
    } catch (error) {
      setRecommendationsError(getErrorMessage(error));
    } finally {
      setRecommendationsLoading(false);
    }
  };

  const loadCompressedFiles = useCallback(async () => {
    try {
      const response = await axios.get(`${API_URL}/compressed-files`);
      setCompressedFiles(response.data?.files || []);
    } catch {
      setCompressedFiles([]);
    }
  }, []);

  useEffect(() => {
    if (activePage === "compression" || activePage === "analytics") {
      loadCompressedFiles();
    }
  }, [activePage, loadCompressedFiles]);

  const compressionAnalytics = useMemo(() => {
    const originalSize = Number(compressionResult?.original_size);
    const compressedSize = Number(compressionResult?.compressed_size);

    const hasResult =
      compressionResult !== null &&
      Number.isFinite(originalSize) &&
      Number.isFinite(compressedSize) &&
      originalSize > 0 &&
      compressedSize >= 0;

    const savedBytes = hasResult
      ? originalSize - compressedSize
      : 0;

    return {
      originalSize: hasResult ? originalSize : 0,
      compressedSize: hasResult ? compressedSize : 0,
      savedBytes,
      savedPercentage: hasResult
        ? (savedBytes / originalSize) * 100
        : 0,
      storageRatio: hasResult
        ? (compressedSize / originalSize) * 100
        : 0,
      hasResult,
    };
  }, [compressionResult]);

  const renderPageTitle = (eyebrow, title, description) => (
    <div className="page-title">
      <div className="eyebrow"><span /> {eyebrow}</div>
      <h1>{title}</h1>
      <p>{description}</p>
    </div>
  );

  const renderDashboard = () => (
    <section className="dashboard-page">
      <section className="hero-section">
        <div className="eyebrow"><span /> YOUR DOCUMENT WORKSPACE</div>
        <h1>Find anything.<br /><span>Faster.</span></h1>
        <p>
          One workspace to search, organize and compress your documents
          with intelligent algorithms.
        </p>

        <button
          className="primary-button"
          onClick={() => goToPage("documents")}
        >
          + Add documents
        </button>

        <form
          className="search-bar dashboard-search"
          onSubmit={(event) => {
            event.preventDefault();
            goToPage("search");
            runSearch(searchQuery);
          }}
        >
          <span className="search-icon">⌕</span>
          <input
            value={searchQuery}
            onChange={(event) => setSearchQuery(event.target.value)}
            placeholder="Search documents, assignments, notes..."
            aria-label="Search documents"
          />
          <button className="primary-button" type="submit">Search →</button>
        </form>
      </section>

      <div className="section-heading">
        <div>
          <h2>Workspace overview</h2>
          <p>A live snapshot of your indexed document collection.</p>
        </div>
        <button className="text-button" onClick={refreshWorkspace}>
          ↻ Refresh
        </button>
      </div>

      {pageError && <div className="error-message">{pageError}</div>}

      <div className="stats-grid">
        <div className="stat-card">
          <span>Total documents</span>
          <strong>{documentCount}</strong>
          <small>Indexed documents</small>
        </div>
        <div className="stat-card">
          <span>Words indexed</span>
          <strong>{Number(totalWords).toLocaleString()}</strong>
          <small>Across your library</small>
        </div>
        <div className="stat-card">
          <span>Library size</span>
          <strong>{formatBytes(totalBytes)}</strong>
          <small>Original text storage</small>
        </div>
        <div className="stat-card">
          <span>Search engine</span>
          <strong className={engineConnected ? "success-text" : "error-text"}>
            {engineConnected ? "Connected" : "Offline"}
          </strong>
          <small>C++ engine status</small>
        </div>
      </div>

      <div className="section-heading">
        <div>
          <h2>Recent documents</h2>
          <p>Open a document to inspect its extracted text.</p>
        </div>
        <button className="text-button" onClick={() => goToPage("documents")}>
          View all →
        </button>
      </div>

      {documents.slice(-5).reverse().map((item) => (
        <div className="document-row" key={getFilename(item)}>
          <div className="file-icon">▤</div>
          <div className="document-row-main">
            <strong>{getFilename(item)}</strong>
            <span>{item.word_count || 0} words · {formatBytes(item.size)}</span>
          </div>
          <button
            className="small-outline-button"
            onClick={() => openDocumentPreview(item)}
          >
            Preview
          </button>
        </div>
      ))}
    </section>
  );

  const renderSearchPage = () => (
    <section className="panel page-panel">
      {renderPageTitle(
        "INTELLIGENT RETRIEVAL",
        "Smart Search",
        "Search documents using C++, BM25 ranking and Trie autocomplete."
      )}

      <form
        className="search-bar"
        onSubmit={(event) => {
          event.preventDefault();
          runSearch();
        }}
      >
        <span className="search-icon">⌕</span>
        <input
          value={searchQuery}
          onChange={(event) => setSearchQuery(event.target.value)}
          placeholder="Search your documents..."
          aria-label="Search query"
        />
        <button className="primary-button" type="submit" disabled={searchLoading}>
          {searchLoading ? "Searching..." : "Search →"}
        </button>
      </form>

      {suggestions.length > 0 && (
        <div className="suggestions-panel">
          <span className="eyebrow">SUGGESTIONS</span>
          {suggestions.map((word) => (
            <button
              className="suggestion-row"
              key={word}
              onClick={() => runSearch(word)}
            >
              <span>⌕</span>
              {word}
              <span>Search ↗</span>
            </button>
          ))}
        </div>
      )}

      {pageError && <div className="error-message">{pageError}</div>}

      <div className="section-heading">
        <h2>Search results</h2>
        <span>{searchResults.length} results</span>
      </div>

      {searchResults.map((item, index) => {
        const filename =
          item.file_name || item.filename || item.fileName || "Unknown document";

        return (
          <div className="document-row" key={`${filename}-${index}`}>
            <div className="result-rank">#{index + 1}</div>
            <div className="document-row-main">
              <strong>{filename}</strong>
              <span>Document ID: {item.document_id ?? "—"}</span>
            </div>
            <div className="result-score">
              <span>RELEVANCE</span>
              <strong>{Number(item.score || 0).toFixed(5)}</strong>
            </div>
            <button
              className="small-outline-button"
              onClick={() => openDocumentPreview(filename)}
            >
              Preview
            </button>
          </div>
        );
      })}

      {!searchLoading && searchQuery && searchResults.length === 0 && !pageError && (
        <div className="empty-state">
          No results yet. Press Search to find matching documents.
        </div>
      )}
    </section>
  );

  const renderDocumentsPage = () => (
    <section className="panel page-panel">
      {renderPageTitle(
        "DOCUMENT LIBRARY",
        "Documents",
        "Upload, organize and preview your searchable text documents."
      )}

      <div className="upload-panel">
        <h2>Upload documents</h2>
        <p>Supported formats are determined by your FastAPI document parser.</p>

        <label className="primary-button upload-button">
          {uploadBusy ? "Uploading..." : "+ Choose files"}
          <input
            type="file"
            multiple
            disabled={uploadBusy}
            accept=".txt,.pdf,.docx,.pptx,.xlsx,.csv"
            onChange={handleUpload}
            hidden
          />
        </label>

        {uploadMessage && <p className="success-text">{uploadMessage}</p>}
      </div>

      {pageError && <div className="error-message">{pageError}</div>}

      <div className="section-heading">
        <h2>All documents</h2>
        <span>{documents.length} files</span>
      </div>

      {documents.map((item) => (
        <div className="document-row" key={getFilename(item)}>
          <div className="file-icon">▤</div>
          <div className="document-row-main">
            <strong>{getFilename(item)}</strong>
            <span>{item.word_count || 0} words · {formatBytes(item.size)}</span>
          </div>
          <button
            className="small-outline-button"
            onClick={() => openDocumentPreview(item)}
          >
            Preview
          </button>
        </div>
      ))}
    </section>
  );

  const renderCompressionPage = () => (
    <section className="panel page-panel">
      {renderPageTitle(
        "LOSSLESS STORAGE",
        "Compression Studio",
        "Compress documents, restore them and verify byte-for-byte integrity."
      )}

      <div className="upload-panel">
        <h2>Compress a document</h2>
        <p>Select a searchable .txt document from your library.</p>

        <select
          value={compressionFilename}
          onChange={(event) => {
            setCompressionFilename(event.target.value);
            setCompressionResult(null);
            setCompressionError("");
          }}
          aria-label="Select document to compress"
        >
          <option value="">Choose a document</option>
          {documents.map((item) => {
            const filename = getFilename(item);
            return <option key={filename} value={filename}>{filename}</option>;
          })}
        </select>

        <button
          className="primary-button"
          onClick={handleCompress}
          disabled={compressionBusy || !compressionFilename}
        >
          {compressionBusy ? "Compressing..." : "Compress document"}
        </button>
      </div>

      {compressionResult && (
        <div className="compression-result">
          <h2>Compression completed</h2>
          <p>
            File: {compressionResult.compressed_file || `${compressionFilename}.huff`}
          </p>
          <div className="stats-grid">
            <div className="stat-card">
              <span>Original size</span>
              <strong>{formatBytes(compressionResult.original_size)}</strong>
            </div>
            <div className="stat-card">
              <span>Compressed size</span>
              <strong>{formatBytes(compressionResult.compressed_size)}</strong>
            </div>
            <div className="stat-card">
              <span>Size reduction</span>
              <strong>
                {Number(compressionResult.compression_percentage ?? 0).toFixed(2)}%
              </strong>
            </div>
          </div>
          <p className="success-text">
            {compressionResult.compressed
              ? "Compression successful."
              : "Compression finished."}
          </p>
        </div>
      )}

      <div className="upload-panel">
        <h2>Restore a compressed document</h2>
        <p>Enter a .huff filename stored in data/compressed.</p>

        <input
          value={restoreFilename}
          onChange={(event) => setRestoreFilename(event.target.value)}
          placeholder="document1.txt.huff"
          aria-label="Compressed filename to restore"
        />

        <button
          className="primary-button"
          onClick={handleRestore}
          disabled={restoreBusy || !restoreFilename.trim()}
        >
          {restoreBusy ? "Restoring..." : "Restore document"}
        </button>
      </div>

      {restoreError && (
  <div className="error-message">{restoreError}</div>
)}

{restoreMessage && (
  <div className="restore-success">
    <p>{restoreMessage}</p>
  </div>
)}

      {restoreResult && (
        <div
          className={
            restoreResult.integrity_verified === true
              ? "restore-success"
              : "restore-integrity-warning"
          }
        >
          <h3>
            {restoreResult.integrity_verified === true
              ? "Integrity verified — lossless restoration confirmed."
              : "Restoration completed; integrity could not be verified."}
          </h3>
          <p>Restored file: {restoreResult.restored_filename}</p>
          <p>Restored size: {formatBytes(restoreResult.restored_size)}</p>

          {restoreResult.integrity_verified === true ? (
            <p>SHA-256 hashes match exactly.</p>
          ) : (
            <p>
              {restoreResult.integrity_message ||
                "Byte-for-byte integrity is unconfirmed."}
            </p>
          )}

          <button
            className="primary-button download-restored-button"
            onClick={handleDownloadRestored}
            disabled={downloadBusy || !restoreResult.restored_filename}
          >
            {downloadBusy ? "Preparing download..." : "↓ Download restored file"}
          </button>
        </div>
      )}

      {compressedFiles.length > 0 && (
        <div className="compressed-files">
          <div className="section-heading">
            <h2>Compressed files</h2>
            <button className="text-button" onClick={loadCompressedFiles}>
              ↻ Refresh
            </button>
          </div>
          {compressedFiles.map((file, index) => {
            const filename =
              typeof file === "string"
                ? file
                : file.filename || file.name || file.file_name;

            if (!filename) return null;

            return (
              <div className="document-row" key={`${filename}-${index}`}>
                <div className="file-icon">▣</div>
                <div className="document-row-main">
                  <strong>{filename}</strong>
                  <span>
                    {file.size != null ? formatBytes(file.size) : "Compressed file"}
                  </span>
                </div>
                <button
                  className="small-outline-button"
                  onClick={() => setRestoreFilename(filename)}
                >
                  Select to restore
                </button>
              </div>
            );
          })}
        </div>
      )}
    </section>
  );

  const renderExplorePage = () => (
    <section className="panel page-panel">
      {renderPageTitle(
        "DOCUMENT DISCOVERY",
        "Explore connections",
        "Discover similar documents using Jaccard similarity, graph connections and BFS traversal in your C++ recommendation engine."
      )}

      <div className="upload-panel">
        <h2>Find related documents</h2>
        <p>
          Choose a numeric document ID from the C++ search engine.
          For example, ID 4 corresponds to document1.txt in your tested dataset.
        </p>

        <label htmlFor="recommendation-document-id">Document ID</label>
        <input
          id="recommendation-document-id"
          type="number"
          min="0"
          max={Math.max(0, documentCount - 1)}
          value={recommendationDocumentId}
          onChange={(event) => {
            setRecommendationDocumentId(event.target.value);
            setRecommendations([]);
            setRecommendationSource(null);
            setRecommendationsError("");
          }}
        />

        <button
          className="primary-button"
          onClick={handleGetRecommendations}
          disabled={recommendationsLoading || recommendationDocumentId === ""}
        >
          {recommendationsLoading ? "Finding documents..." : "Find recommendations"}
        </button>
      </div>

      {recommendationsError && (
        <div className="error-message">{recommendationsError}</div>
      )}

      {recommendationSource && (
        <div className="recommendation-results">
          <div className="section-heading">
            <div>
              <h2>Similar documents</h2>
              <p>
                Based on <strong>{recommendationSource.document_name}</strong>
              </p>
            </div>
            <span>
              {recommendationSource.recommendation_count ?? recommendations.length} results
            </span>
          </div>

          {recommendations.length === 0 ? (
            <div className="empty-state">
              No similar documents found. Try another document ID.
            </div>
          ) : (
            recommendations.map((item, index) => {
              const similarity = Number(item.similarity);
              const percentage = Number.isFinite(similarity)
                ? `${(similarity * 100).toFixed(2)}%`
                : "—";

              return (
                <div
                  className="document-row"
                  key={`${item.document_id}-${item.file_name}`}
                >
                  <div className="result-rank">#{index + 1}</div>
                  <div className="document-row-main">
                    <strong>{item.file_name}</strong>
                    <span>Document ID: {item.document_id}</span>
                  </div>
                  <div className="result-score">
                    <span>SIMILARITY</span>
                    <strong>{percentage}</strong>
                  </div>
                  <button
                    className="small-outline-button"
                    onClick={() => openDocumentPreview(item.file_name)}
                  >
                    Preview
                  </button>
                </div>
              );
            })
          )}
        </div>
      )}
    </section>
  );

  const renderFeaturePage = (page) => {
    const details = PAGE_DETAILS[page];
    if (!details) return null;

    return (
      <section className="panel page-panel">
        {renderPageTitle(details.eyebrow, details.title, details.description)}

        {page === "analytics" && (
          <div className="analytics-workspace">
            <div className="stats-grid">
              <div className="stat-card">
                <span>Total documents</span>
                <strong>{documentCount}</strong>
                <small>Documents in your library</small>
              </div>
              <div className="stat-card">
                <span>Words indexed</span>
                <strong>{Number(totalWords).toLocaleString()}</strong>
                <small>Total indexed words</small>
              </div>
              <div className="stat-card">
                <span>Library size</span>
                <strong>{formatBytes(totalBytes)}</strong>
                <small>Original document storage</small>
              </div>
              <div className="stat-card">
                <span>Search engine</span>
                <strong>{engineConnected ? "Connected" : "Offline"}</strong>
                <small>C++ engine status</small>
              </div>
            </div>

            <section className="analytics-panel">
              <div className="analytics-panel-heading">
                <div>
                  <span className="eyebrow">STORAGE INTELLIGENCE</span>
                  <h2>Compression performance</h2>
                  <p>Compare original and compressed file sizes.</p>
                </div>
                <button
                  className="small-outline-button"
                  onClick={loadCompressedFiles}
                >
                  ↻ Refresh
                </button>
              </div>

              {!compressionAnalytics.hasResult ? (
                <div className="analytics-empty">
                  <div className="large-feature-icon">▣</div>
                  <h3>No compression result available</h3>
                  <p>
                    The latest compression result is held in the current
                    session. Compress a document to view its size comparison.
                  </p>
                  <button
                    className="primary-button"
                    onClick={() => goToPage("compression")}
                  >
                    Open Compression Studio
                  </button>
                </div>
              ) : (
                <>
                  <div className="analytics-size-row">
                    <div>
                      <span>Original size</span>
                      <strong>{formatBytes(compressionAnalytics.originalSize)}</strong>
                    </div>
                    <div>
                      <span>Compressed size</span>
                      <strong>{formatBytes(compressionAnalytics.compressedSize)}</strong>
                    </div>
                  </div>

                  <div className="analytics-bar-label">
                    <span>Original size</span>
                    <strong>100%</strong>
                  </div>
                  <div className="analytics-bar">
                    <div
                      className="analytics-bar-fill original-fill"
                      style={{ width: "100%" }}
                    />
                  </div>

                  <div className="analytics-bar-label">
                    <span>Compressed size remaining</span>
                    <strong>
                      {compressionAnalytics.storageRatio.toFixed(2)}%
                    </strong>
                  </div>
                  <div className="analytics-bar">
                    <div
                      className="analytics-bar-fill compressed-fill"
                      style={{
                        width: `${Math.min(100, Math.max(0, compressionAnalytics.storageRatio))}%`,
                      }}
                    />
                  </div>

                  <div className="analytics-savings-grid">
                    <div className="analytics-savings-card">
                      <span>Space saved</span>
                      <strong>{formatBytes(compressionAnalytics.savedBytes)}</strong>
                    </div>
                    <div className="analytics-savings-card">
                      <span>Size reduction</span>
                      <strong>
                        {compressionAnalytics.savedPercentage.toFixed(2)}%
                      </strong>
                    </div>
                  </div>

                  {compressionAnalytics.savedBytes < 0 && (
                    <p className="analytics-note">
                      This file became larger after compression. The compressed
                      format has overhead for this input.
                    </p>
                  )}

                  <p className="analytics-note">
                    These figures describe the latest compression operation,
                    not aggregate statistics for all compressed files.
                  </p>
                </>
              )}

              {compressedFiles.length > 0 && (
                <div className="analytics-compressed-list">
                  <div className="section-heading">
                    <h3>Compressed files found</h3>
                    <span>{compressedFiles.length} files</span>
                  </div>
                  {compressedFiles.map((file, index) => {
                    const filename =
                      typeof file === "string"
                        ? file
                        : file.filename || file.name || file.file_name;

                    if (!filename) return null;

                    return (
                      <div className="document-row" key={`${filename}-${index}`}>
                        <div className="file-icon">▣</div>
                        <div className="document-row-main">
                          <strong>{filename}</strong>
                          <span>
                            {file.size != null ? formatBytes(file.size) : "Compressed file"}
                          </span>
                        </div>
                      </div>
                    );
                  })}
                </div>
              )}
            </section>
          </div>
        )}

        {page === "settings" && (
          <div className="settings-row">
            <div>
              <strong>Appearance</strong>
              <p>Switch between dark and light appearance.</p>
            </div>
            <button
              className="small-outline-button"
              onClick={() => setIsDark((current) => !current)}
            >
              Switch to {isDark ? "light" : "dark"} mode
            </button>
          </div>
        )}

        {details.features.map((feature) => (
          <div className="document-row" key={feature}>
            <div className="file-icon">{details.icon}</div>
            <div className="document-row-main">
              <strong>{feature}</strong>
              <span>SmartFile module</span>
            </div>
          </div>
        ))}
      </section>
    );
  };

  return (
    <div className={`app-shell ${isDark ? "" : "light-theme"}`}>
      <aside className="sidebar">
        <div className="brand">
          <div className="brand-mark">S</div>
          <div>
            <div className="brand-name">SmartFile</div>
            <div className="brand-caption">DOCUMENT INTELLIGENCE</div>
          </div>
        </div>

        <div className="sidebar-label">WORKSPACE</div>

        <nav className="sidebar-nav">
          {NAV_ITEMS.map((item) => (
            <button
              key={item.id}
              className={
                activePage === item.id ? "sidebar-link active" : "sidebar-link"
              }
              onClick={() => goToPage(item.id)}
            >
              <span className="sidebar-icon">{item.icon}</span>
              <span>{item.label}</span>
              {activePage === item.id && <span className="active-dot" />}
            </button>
          ))}
        </nav>

        <div className="engine-card">
          <div className="engine-card-title">
            <span className={engineConnected ? "status-dot online" : "status-dot"} />
            C++ Search Engine
          </div>
          <p>
            {engineConnected
              ? `${documentCount} documents available`
              : "Connection unavailable"}
          </p>
        </div>

        <button
          className="appearance-toggle"
          onClick={() => setIsDark((current) => !current)}
          aria-label="Toggle appearance"
        >
          <span>{isDark ? "☾" : "☀"}</span>
          <span>{isDark ? "Dark appearance" : "Light appearance"}</span>
          <span className={`toggle-track ${isDark ? "on" : ""}`}>
            <span className="toggle-knob" />
          </span>
        </button>

        <div className="sidebar-footer">
          <span>SMARTFILE WORKSPACE</span>
          <p>React · FastAPI · C++</p>
        </div>
      </aside>

      <main className="main-area">
        <header className="topbar">
          <div className="breadcrumb">
            <span>Workspace</span>
            <span>/</span>
            <strong>
              {NAV_ITEMS.find((item) => item.id === activePage)?.label || "Dashboard"}
            </strong>
          </div>

          <div className="topbar-actions">
            <div className="connection-status">
              <span className={engineConnected ? "status-dot online" : "status-dot"} />
              {engineConnected ? "Engine connected" : "Engine disconnected"}
            </div>
            <button
              className="icon-button"
              onClick={() => setIsDark((current) => !current)}
              aria-label="Toggle theme"
              title="Toggle theme"
            >
              {isDark ? "☼" : "☾"}
            </button>
            <div className="profile-avatar">SF</div>
          </div>
        </header>

        <div className="page-content">
          {loading && documents.length === 0 && (
            <div className="empty-state">Loading your workspace...</div>
          )}

          {activePage === "dashboard" && renderDashboard()}
          {activePage === "search" && renderSearchPage()}
          {activePage === "documents" && renderDocumentsPage()}
          {activePage === "compression" && renderCompressionPage()}
          {activePage === "explore" && renderExplorePage()}
          {PAGE_DETAILS[activePage] && renderFeaturePage(activePage)}
        </div>
      </main>

      {previewDocument && (
        <div
          className="preview-overlay"
          onMouseDown={(event) => {
            if (event.target === event.currentTarget) {
              setPreviewDocument(null);
            }
          }}
        >
          <section
            className="preview-modal"
            role="dialog"
            aria-modal="true"
            aria-label="Document preview"
          >
            <header className="preview-header">
              <div>
                <strong>{previewDocument.filename}</strong>
                <p>Extracted document text</p>
              </div>
              <button
                className="icon-button"
                onClick={() => setPreviewDocument(null)}
                aria-label="Close preview"
              >
                ×
              </button>
            </header>

            <div className="preview-body">
              {previewLoading ? (
                <div className="empty-state">Loading document...</div>
              ) : (
                <pre>{previewDocument.content}</pre>
              )}
            </div>
          </section>
        </div>
      )}
    </div>
  );
}
