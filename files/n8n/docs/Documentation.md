# HTL Cloud – Zentrale IT-Support & System-Dokumentation

Diese Dokumentation dient als zentrale Wissensbasis für den IT-Service-Desk der **HTL Cloud**. Sie definiert betreute Dienste, Pflichtangaben für Support-Tickets, bekannte Fehlerbilder, Berechtigungsregeln und die interne Team-Zuweisung.

---

## 1. Allgemeine Triage- & Pflichtangaben-Checkliste

Damit ein Support-Ticket ohne Rückfragen an ein Fachteam übergeben werden kann, müssen folgende Grunddaten vorliegen:
1. **Betroffener Dienst / System:** Exakter Name (z. B. *HTL-OpenVPN*, *Nextcloud*, *Moodle*, *GitLab*, *Eduroam*, *Proxmox*). Unklare Angaben wie „Schulnetz“ oder „Cloud“ reichen nicht aus.
2. **Benutzerkennung / Kürzel:** Active-Directory-Benutzername (z. B. `m.schmidt` oder Lehrerkürzel).
3. **Exakte Fehlermeldung / Symptom:** Wortlaut der Fehlermeldung, HTTP-Statuscode oder beigefügter Screenshot/Log-Auszug.
4. **Zeitpunkt des Auftretens:** Datum und ungefähre Uhrzeit (für Server- und Firewall-Logs).
5. **Orts- bzw. Zielangabe:** Raum-/Labornummer (bei Hardware/WLAN) bzw. konkreter Kurs-, VM- oder Projektname.

### Berechtigungs- & Eskalationsregeln
- **Keine Admin-/Root-Rechte für Schüler:** Schüler erhalten grundsätzlich keinen `root`- oder lokalen Administrator-Zugriff auf Produktivserver oder Labor-Hosts.
- **Freigabepflicht:** Firewall-Freischaltungen, Server-Konfigurationsänderungen, Massen-Account-Erstellungen oder Kontingenterhöhungen über 25 GB benötigen die explizite Bestätigung (oder CC) einer **Lehrkraft** bzw. des **Abteilungsvorstands**.

---

## 2. HTL-OpenVPN & Fernzugriff
- **Zuständiges Team:** `Network & Security`
- **Zugangsberechtigung:**
  - **Lehrkräfte & Verwaltung:** Standardmäßig freigeschaltet.
  - **Schüler:** Standardmäßig nur für **5. Jahrgänge (Diplomarbeiten)** oder nach Freigabe durch eine Betreuungslehrkraft aktiviert.
- **Konfiguration:** Aktuelles `.ovpn`-Profil aus dem HTL-Portal erforderlich (Client-Zertifikate laufen jährlich am **30. September** ab).

### Bekannte Fehlerbilder (OpenVPN)
- **`TLS Key Negotiation Timeout`:**
  - *Ursache:* UDP-Port 1194 wird im lokalen Netzwerk blockiert (z. B. öffentliches WLAN, Hotel, Mobilfunk-Provider) oder das Zertifikat ist abgelaufen.
  - *Benötigte Infos:* Betriebssystem, OpenVPN-Client-Version, Ausstellungsdatum des `.ovpn`-Profils und ob ein Gegentest über ein anderes Netzwerk (z. B. Smartphone-Hotspot) durchgeführt wurde.
- **`AUTH_FAILED` / `Authentifizierung verweigert`:**
  - *Ursache:* Falsches AD-Passwort, gesperrter Account oder fehlende Mitgliedschaft in der AD-Gruppe `VPN-Users`.
  - *Benötigte Infos:* Benutzerkürzel, ob der Login in Moodle/M365 funktioniert und seit wann der Fehler auftritt.

---

## 3. Nextcloud (`cloud.htl-musterstadt.at`)
- **Zuständiges Team:** `Cloud & Virtualization`
- **Standard-Speicherkontingente (Quota):**
  - Schüler: `10 GB`
  - Diplomarbeitsteams (Gruppenordner): `25 GB`
  - Lehrkräfte: `50 GB`

### Pflichtangaben & Fehlerbilder (Nextcloud)
- **Speichererweiterung anfragen:** Benötigt (1) gewünschte Zielgröße in GB, (2) konkrete Begründung (z. B. Diplomarbeit, Videoprojekt) und (3) bei Schülern den Namen der betreuenden Lehrkraft.
- **`507 Insufficient Storage`:** Persönlicher Speicherplatz ist erschöpft (Hinweis: auch den Nextcloud-Papierkorb leeren).
- **`403 Forbidden` beim Hochladen/Bearbeiten:** Fehlende Schreibrechte in einem geteilten Ordner -> Ordnerbesitzer oder IT-Support muss Rechte anpassen (Ordnername und Besitzer angeben).

---

## 4. Moodle & Microsoft 365 / Teams
- **Zuständiges Team:** `Identity & Access`
- **Kurseinschreibungen (Moodle):** Synchronisation mit Untis/Sokrates erfolgt automatisch einmal täglich (Änderungen dauern bis zu 24 Stunden).
- **Pflichtangaben bei fehlenden Kursen oder Teams:** Exakter **Kurs-/Teamname**, **Klasse/Jahrgang** (z. B. `4AHIF`) und Name der **unterrichtenden Lehrkraft**.
- **Passwort-Reset:** Kann über das Self-Service-Portal oder durch eine Lehrkraft im Unterricht zurückgesetzt werden; bei gesperrtem AD-Konto ist der vollständige Name und das Geburtsdatum bzw. die Schüler-ID zur Verifizierung nötig.

---

## 5. GitLab, Proxmox & Docker-Umgebungen
- **Zuständiges Team:** `Cloud & Virtualization`
- **Erreichbarkeit:** GitLab (`gitlab.htl-musterstadt.at`) und Proxmox-Cluster sind aus Sicherheitsgründen **nur aus dem internen Schulnetz oder über HTL-OpenVPN** erreichbar.
- **Pflichtangaben für Tickets:**
  - **GitLab:** Projekt-/Repository-Pfad, Benutzerkürzel und exakte Git-Fehlermeldung.
  - **Proxmox / VMs / Docker:** VM-ID oder Hostname, betroffener Cluster/Labor-Host, gewünschte Ressourcen (CPU/RAM/Disk) bei Neuanlage sowie die vollständige Konsolen-Fehlermeldung.

---

## 6. Eduroam WLAN
- **Zuständiges Team:** `Network & Security`
- **Anmeldeformat:** Immer vollständige Schul-E-Mail-Adresse verwenden (`vorname.nachname@htl-musterstadt.at` bzw. `@students.htl-musterstadt.at`), **nicht** nur das Benutzerkürzel.
- **Häufiger Fehler nach Passwortänderung:** Das alte Passwort ist auf dem Endgerät gespeichert -> WLAN-Profil „eduroam“ entfernen/vergessen und neu verbinden.
- **Pflichtangaben für WLAN-Störungen:** Raum/Trakt (z. B. *3. Stock Nord*, *Labor 204*), Endgerätetyp/Betriebssystem und Uhrzeit des Verbindungsabbruchs.

---

## 7. EDV-Labore, Hardware & Software-Lizenzen
- **Zuständige Teams:**
  - `Lab & Hardware`: Labor-PCs, Monitore, Beamer, Smartboards, Drucker, Peripherie.
  - `Software & Licensing`: IDEs (JetBrains, VS Code), CAD (AutoCAD, SolidWorks), Adobe Creative Cloud, Betriebssystem-Images.
- **Pflichtangaben für Labor- & Software-Tickets:**
  1. Exakte **Raum-/Labornummer** (z. B. `Labor 302`).
  2. **Arbeitsplatz-Nummer / Hostname** (Aufkleber am PC/Monitor, z. B. `PC-302-14`), sofern nicht der gesamte Saal betroffen ist.
  3. **Softwarename & benötigte Version** (bei Installations- oder Lizenzanfragen).

---

## 8. Übersicht: Interne Support-Queues (Routing)

| Queue (`assigned_team`) | Zuständigkeitsbereich |
| :--- | :--- |
| **Network & Security** | HTL-OpenVPN, Eduroam WLAN, Firewall, VLANs, DNS, Switches |
| **Cloud & Virtualization** | Nextcloud, Proxmox, Docker, GitLab, VMs, Speicherplatz |
| **Identity & Access** | Active Directory, Passwörter, Berechtigungen, M365/Teams, Moodle |
| **Lab & Hardware** | Labor-PCs, Beamer, Smartboards, Drucker, Raum-Hardware |
| **Software & Licensing** | IDEs, CAD-Software, Adobe, JetBrains, Software-Installationen |
| **Service Desk General** | Allgemeine Anfragen, unklare Zuständigkeit |