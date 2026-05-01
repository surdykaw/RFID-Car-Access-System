# Vehicle Integration

This document describes how the RFID access control module was integrated with the vehicle's central locking system.

> [!Note]
> The terms *controller* and *decoder* refer to the same central locking module in the vehicle.

The goal was to add RFID-based lock/unlock functionality without modifying the internal electronics of the vehicle's central locking controller.

Instead of controlling the door lock actuators directly, the system electrically simulates the original lock/unlock signals for the central locking button located inside the vehicle cabin.

The integration described in this document was performed on a vehicle equipped with the following central locking decoder:

**Manufacturer:** VALEO  
**Decoder model:** 73847257

---
# Central Lock Button Interface

The vehicle includes a physical button on the dashboard that allows locking and unlocking all doors.

The button works by closing specific control lines connected to the central locking controller.

Based on a service documentation analysis, the relevant signals were identified as:

| Pin | Function |
|----|----|
| ?? | Unlock command |
| ?? | Lock command |

*TO DO*

Pressing the button connects the control line to ground, which is interpreted by the decoder as a lock or unlock request.

The RFID access control system replicates this behavior by momentarily closing the same control lines.

---
# Integration Point

The access control system module is connected in parallel with the original lock/unlock button wiring.

Two control lines are used:

- **Unlock line**
- **Lock line**

The original button remains fully functional because the RFID module only momentarily closes the same electrical path used by the button.

---
# Transistor Switching Interface

The connection between the microcontroller and the vehicle control lines is implemented using two **NPN transistor switches**.

Each transistor acts as a low-side switch controlled by the microcontroller.

This is electrically equivalent to pressing the physical dashboard button.

The firmware keeps the transistor active for approximately **1 second**, which replicates a typical manual button press.

---
# Power Supply

The access system module is powered directly from the **12V supply available at the central locking controller**.

The internal electronics of the RFID system operate at **3.3V**, therefore a voltage regulator is used.

Power path
```
Vehicle 12V
↓
LD1117 voltage regulator
↓
3.3V system supply
↓
ATmega8A + MFRC522
```

---
# Design Considerations
The integration method was chosen to minimize risk and avoid modifying the original vehicle electronics.

Key properties of this approach:

- no modification of the central locking controller
- original dashboard button remains functional
- the RFID module only simulates an existing control input
- electrical interface implemented using transistor switches

This ensures that the system behaves like a standard control input rather than an external actuator driver.

---
# Notes
The exact wiring and signal logic may vary between vehicle models.

The integration described here is specific to the vehicle model used during development.
Adapting the system to another vehicle requires analyzing its central locking control scheme and identifying the corresponding control lines.