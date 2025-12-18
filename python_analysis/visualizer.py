import subprocess
import os
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# --- KONFIGURACJA ---
# Zmień tę ścieżkę na dokładną lokalizację Twojego skompilowanego pliku .exe
EXECUTABLE_DIR = '../cpp_sim/cmake-build-debug'
EXECUTABLE_NAME = 'sm_sim.exe' # lub 'sm_sim' na Linuksie
EXECUTABLE_PATH = os.path.join(EXECUTABLE_DIR, EXECUTABLE_NAME)


# Sprawdzenie, czy plik istnieje
if not os.path.exists(EXECUTABLE_PATH):
    print(f"Error: Executable not found at '{EXECUTABLE_PATH}'")
    print("Please compile the C++ project and check the path.")
    exit()

# Ustawienia wykresu
MAX_POINTS = 100  # Ile punktów historii pokazywać na wykresie

# Przygotuj dane
time_steps = deque(maxlen=MAX_POINTS)
best_bids = deque(maxlen=MAX_POINTS)
best_asks = deque(maxlen=MAX_POINTS)

# Inicjalizacja figury i osi wykresu
fig, ax = plt.subplots()
bid_line, = ax.plot([], [], 'g-', label='Best Bid')
ask_line, = ax.plot([], [], 'r-', label='Best Ask')

current_time = 0

# Uruchom symulację C++ jako podproces
# stdout=subprocess.PIPE -> Przechwytujemy standardowe wyjście
# text=True -> Dane będą traktowane jako tekst (dekodowane z UTF-8)
# bufsize=1 -> Tryb buforowania liniowego, dane pojawiają się natychmiast po każdej nowej linii
process = subprocess.Popen(EXECUTABLE_PATH, stdout=subprocess.PIPE, text=True, bufsize=1,cwd=EXECUTABLE_DIR)


def init():
    """Funkcja inicjalizująca wykres."""
    ax.set_xlim(0, MAX_POINTS)
    ax.set_ylim(95, 105)
    ax.legend()
    ax.set_xlabel("Time Steps")
    ax.set_ylabel("Price")
    ax.grid(True)
    return bid_line, ask_line,


def update(frame):
    """Funkcja aktualizująca dane na wykresie."""
    global current_time
    try:
        # Czytaj jedną linię z wyjścia podprocesu
        line = process.stdout.readline()
        if line:
            print(f"Odebrano: '{line.strip()}'")
        # ------------------------------------

        if not line:
            # Jeśli proces C++ się zakończył, zatrzymaj animację
            ani.event_source.stop()
            return bid_line, ask_line,

        parts = line.strip().split(',')
        if len(parts) == 2:
            bid, ask = float(parts[0]), float(parts[1])

            time_steps.append(current_time)
            best_bids.append(bid)
            best_asks.append(ask)

            bid_line.set_data(time_steps, best_bids)
            ask_line.set_data(time_steps, best_asks)

            if current_time >= MAX_POINTS:
                ax.set_xlim(current_time - MAX_POINTS + 1, current_time + 1)

            if len(best_bids) > 1:
                min_price = min(best_bids)
                max_price = max(best_asks)
                padding = max(0.1, (max_price - min_price) * 0.1)
                ax.set_ylim(min_price - padding, max_price + padding)

            current_time += 1
    except (ValueError, IndexError):
        pass

    return bid_line, ask_line,


def on_close(event):
    """Funkcja wywoływana przy zamknięciu okna wykresu."""
    print("Closing plot window, terminating C++ simulation...")
    process.terminate()  # Zakończ proces C++


# Podłącz funkcję on_close do zdarzenia zamknięcia okna
fig.canvas.mpl_connect('close_event', on_close)

# Uruchom animację
ani = animation.FuncAnimation(fig, update, init_func=init, blit=True, interval=1, cache_frame_data=False)

plt.show()

# Opcjonalnie: poczekaj na zakończenie procesu po zamknięciu okna
try:
    process.wait(timeout=5)
except subprocess.TimeoutExpired:
    print("Process did not terminate gracefully, killing it.")
    process.kill()

print("Visualization finished.")