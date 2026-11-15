from pathlib import Path
import uvicorn
from mcp.server.mcpserver import MCPServer
from mcp.server.transport_security import TransportSecuritySettings

# Line 5: ONLY pass the name here — no host or port!
mcp = MCPServer("HTL Cloud Documentation Server")

DOCS_DIR = Path("./docs").resolve()
DOCS_DIR.mkdir(parents=True, exist_ok=True)


def _safe_path(relative_path: str) -> Path:
    """Ensure the requested file stays strictly inside DOCS_DIR."""
    target = (DOCS_DIR / relative_path).resolve()
    if not str(target).startswith(str(DOCS_DIR)):
        raise ValueError("Access denied: Path escapes the documentation directory.")
    return target


@mcp.tool()
def list_docs() -> list[str]:
    """List all available documentation files in the HTL Cloud docs folder."""
    files = [
        str(p.relative_to(DOCS_DIR))
        for p in DOCS_DIR.rglob("*")
        if p.is_file() and p.suffix.lower() in {".md", ".txt", ".json"}
    ]
    return sorted(files)


@mcp.tool()
def read_doc(filename: str) -> str:
    """Read the full contents of a documentation file (e.g., 'vpn_guide.md')."""
    file_path = _safe_path(filename)
    if not file_path.exists():
        return f"Error: File '{filename}' not found. Call list_docs() first."
    return file_path.read_text(encoding="utf-8")


@mcp.tool()
def write_markdown_doc(filename: str, content: str, overwrite: bool = True) -> str:
    """Create or update a Markdown (.md) documentation file in the docs folder."""
    if not filename.endswith(".md"):
        filename += ".md"

    file_path = _safe_path(filename)
    if file_path.exists() and not overwrite:
        return f"Skipped: '{filename}' already exists and overwrite=False."

    file_path.parent.mkdir(parents=True, exist_ok=True)
    file_path.write_text(content, encoding="utf-8")
    return f"Successfully saved documentation to '{filename}' ({len(content)} chars)."


if __name__ == "__main__":
    # Build the Streamable HTTP ASGI app and allow connections from Docker (host.docker.internal)
    app = mcp.streamable_http_app(
        stateless_http=True,
        transport_security=TransportSecuritySettings(
            enable_dns_rebinding_protection=False
        ),
    )
    uvicorn.run(app, host="0.0.0.0", port=8000)