#pragma once

#include <stdexcept>

// Simuliertes Geraet fuer Tests -- keine echte GPU-/DirectX12-Anbindung.
// Zaehlt begin()/end()-Aufrufe und erkennt Fehlbenutzung (doppeltes begin()
// ohne end(), end() ohne vorheriges begin()), damit Tests das Verhalten von
// ScopedGuard unter regulaerem Ablauf und unter einer Exception mitten im
// Scope pruefen koennen.

class MockDevice {
public:
    void begin() {
        if (open_) {
            throw std::logic_error("begin() called while already open");
        }
        open_ = true;
        ++begin_count_;
    }

    void end() {
        if (!open_) {
            throw std::logic_error("end() called while not open");
        }
        open_ = false;
        ++end_count_;
    }

    bool is_open() const { return open_; }
    int begin_count() const { return begin_count_; }
    int end_count() const { return end_count_; }

private:
    bool open_ = false;
    int begin_count_ = 0;
    int end_count_ = 0;
};
