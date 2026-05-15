# Solar Power Management System

An intelligent IoT solar monitoring solution with integrated AI-powered Retrieval-Augmented Generation (RAG) for natural language insights.

## 🚀 Overview
This system provides real-time monitoring of solar electricity generation and usage. It features an ESP32-based edge sensing layer, a Raspberry Pi gateway for data processing, and a RAG-based AI assistant to answer questions about system performance and maintenance.

### Key Features
- **Real-time Monitoring**: Track voltage, current, power, and energy generation.
- **AI Assistant**: Query system performance and manuals using natural language (e.g., "Why was generation low yesterday?").
- **Local Analytics**: Optimized for Raspberry Pi 3B+ using lightweight time-series and vector databases.
- **Open Source**: Built with open-source components including MQTT, SQLite, and FastAPI.

## 📁 Project Structure
- `firmware/`: ESP32 source code for electrical sensing and telemetry.
- `backend/`: Python-based gateway, MQTT broker configuration, and RAG pipeline.
- `frontend/`: Web dashboard for visualization and AI interaction.
- `docs/`: Design documents, architecture diagrams, and parameter mappings.

## 🛠 Tech Stack
- **Edge**: ESP32, INA219/INA226 Sensors, MQTT.
- **Gateway**: Raspberry Pi 3B+, Mosquitto, FastAPI.
- **AI**: SQLite (`sqlite-vec`), Groq/Gemini API for LLM.
- **Visualization**: Chart.js, HTML5/CSS3.

## ⚙️ Setup
Detailed setup instructions for each component can be found in their respective directories.
1. **Hardware**: See `firmware/README.md`.
2. **Gateway**: See `backend/README.md`.
3. **Frontend**: See `frontend/README.md`.

---
*Designed for efficiency, built for insights.*
