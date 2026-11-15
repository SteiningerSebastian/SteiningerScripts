# AI Agents Lab — Starter Kit

This starter kit contains the container stack, workflow export, and Python MCP server used in the **AI Agents** laboratory.

## Contents

- `docker-compose.yaml` — Multi-container stack running **n8n**, task **runners**, **sandbox** execution containers, and **SearXNG**.
- `.env` — Pre-configured environment variables and lab secrets for local testing.
- `searxng-settings.yml` — Search engine configuration for the bundled SearXNG service.
- `HTLCloudTicketSystem.json` — Exportable n8n workflow for the HTL Cloud IT Support Ticket System.
- `Worflow.png` — Visual overview of the HTL Cloud Ticket System workflow.
- `server.py` — Custom Python MCP server exposing local documentation tools over Streamable HTTP (`port 8000`).
- `requirements.txt` — Python dependencies required for `server.py`.
- `docs/Documentation.md` — Knowledge base used by the Triage Agent via MCP.

## Quick Start

1. Start the n8n stack with Docker Compose:

   ```powershell
   docker compose up -d
   ```

2. Open n8n in your browser at <http://localhost:5678/>.

3. Create a Python virtual environment on your host machine and start the MCP server:

   ```powershell
   python -m venv .venv
   .\.venv\Scripts\Activate.ps1
   pip install -r requirements.txt
   python server.py
   ```
