
```mermaid
flowchart LR
    %% Styling der Blöcke
    classDef ext fill:#f9f9f9,stroke:#666,stroke-width:2px,color:#333
    classDef switch fill:#ffebee,stroke:#c62828,stroke-width:2px,color:#b71c1c
    classDef ctrl fill:#2b4a7d,stroke:#1a2b4c,stroke-width:2px,color:#fff
    classDef power fill:#e7f9e7,stroke:#2b7d4a,stroke-width:2px,color:#1a4c2b
    classDef config fill:#fff3e0,stroke:#d48806,stroke-width:2px,color:#875604,stroke-dasharray: 5 5

    %% Nodes
    USBC["<b>USB-C Buchse</b>"]:::ext
    Switch{"<b>Schalter</b>"}:::switch
    Matrix[/"<b>Widerstandsmatrix</b><br/>(Spannungs- & Stromlimit)"/]:::config
    PDCtrl{{"<b>USB-PD Controller</b>"}}:::ctrl
    
    Reg5V["<b>Hauptregler</b><br/>(5V)"]:::power
    
    subgraph Verteilung ["Spannungsverteilung"]
        direction TB
        StepUp["<b>Step-Up Wandler</b><br/>(12V / 24V)"]:::power
        StepDown["<b>Step-Down Wandler</b><br/>(3,3V / 1,8V)"]:::power
    end

    %% Verbindungen
    USBC <==> Switch
    Switch <==>|"PD-Protokoll<br/>Aushandlung"| PDCtrl
    
    Matrix -.->|"konfiguriert"| PDCtrl
    
    PDCtrl ===>|"VBUS<br/>(max. 15V / 3,9 A)"| Reg5V
    
    Reg5V ===>|5V| StepUp
    Reg5V ===>|5V| StepDown