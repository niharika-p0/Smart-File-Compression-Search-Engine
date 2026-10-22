
const renderExplorePage = () => (
  <section className="panel page-panel">
    <div className="page-title">
      <div className="eyebrow">
        <span /> DOCUMENT DISCOVERY
      </div>

      <h1>Explore connections</h1>

      <p>
        Discover similar documents using Jaccard similarity,
        graph connections, and BFS traversal in your C++
        recommendation engine.
      </p>
    </div>

    <div className="upload-panel">
      <h2>Find related documents</h2>

      <p>
        Select a document from your indexed collection to
        discover related documents.
      </p>

      <label htmlFor="recommendation-document-id">
        Select document
      </label>

      <select
        id="recommendation-document-id"
        value={recommendationDocumentId}
        onChange={(event) => {
          setRecommendationDocumentId(event.target.value);
          setRecommendations([]);
          setRecommendationSource(null);
          setRecommendationsError("");
        }}
        aria-label="Select document for recommendations"
      >
        <option value="">Choose a document</option>

        {documents.map((item, index) => {
          const filename =
            item.stored_filename ||
            item.filename ||
            item.file_name ||
            item.fileName ||
            item.original_filename;

          const documentId =
            item.document_id ??
            item.documentId ??
            item.id;

          if (
            documentId === undefined ||
            documentId === null ||
            !filename
          ) {
            return null;
          }

          return (
            <option
              key={`${documentId}-${filename}-${index}`}
              value={String(documentId)}
            >
              {filename} (ID: {documentId})
            </option>
          );
        })}
      </select>

      <button
        className="primary-button"
        onClick={handleGetRecommendations}
        disabled={
          recommendationsLoading ||
          !recommendationDocumentId
        }
      >
        {recommendationsLoading
          ? "Finding similar documents..."
          : "Find recommendations"}
      </button>
    </div>

    {recommendationsError && (
      <div className="error-banner" role="alert">
        {recommendationsError}
      </div>
    )}

    {recommendationSource && (
      <div className="panel compression-result">
        <div className="section-heading-row">
          <div>
            <h2>Similar documents</h2>

            <p>
              Based on{" "}
              <strong>
                {recommendationSource.document_name ||
                  `Document ${recommendationSource.document_id}`}
              </strong>
            </p>
          </div>

          <span>
            {recommendationSource.recommendation_count ??
              recommendations.length}{" "}
            results
          </span>
        </div>

        {!recommendations.length ? (
          <div className="empty-state">
            No similar documents found. Try another document.
          </div>
        ) : (
          recommendations.map((item, index) => {
            const similarity = Number(item.similarity);

            const percentage = Number.isFinite(similarity)
              ? `${(similarity * 100).toFixed(2)}%`
              : "—";

            const filename =
              item.file_name ||
              item.filename ||
              item.fileName ||
              "Unnamed document";

            return (
              <div
                className="document-row"
                key={`${item.document_id}-${filename}`}
              >
                <div className="result-rank">
                  #{index + 1}
                </div>

                <div className="document-row-main">
                  <strong>{filename}</strong>

                  <span>
                    Document ID: {item.document_id ?? "—"}
                  </span>
                </div>

                <div className="result-score">
                  <span>SIMILARITY</span>
                  <strong>{percentage}</strong>
                </div>

                <button
                  className="small-outline-button"
                  onClick={() =>
                    openDocumentPreview({
                      filename,
                      stored_filename: filename,
                      file_name: filename,
                    })
                  }
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


export default App;
