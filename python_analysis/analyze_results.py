import pandas as pd
import matplotlib.pyplot as plt
import os

# --- KONFIGURACJA ---
LOG_FILE = "../cpp_sim/logs/market.csv"  # Upewnij się, że ścieżka jest poprawna względem miejsca uruchomienia skryptu
WINDOW_SIZE = 100  # Uśrednianie z ilu kroków? (Im więcej, tym gładszy wykres)


def analyze():
    # 1. Sprawdzenie czy plik istnieje
    if not os.path.exists(LOG_FILE):
        print(f"BŁĄD: Nie znaleziono pliku: {os.path.abspath(LOG_FILE)}")
        return

    print("Wczytywanie danych...")
    try:
        df = pd.read_csv(LOG_FILE)
    except Exception as e:
        print(f"Błąd odczytu pliku CSV: {e}")
        return

    # Obliczenia pomocnicze
    df['MidPrice'] = (df['BestBid'] + df['BestAsk']) / 2.0

    # Obliczamy różnicę między ceną rynkową a fundamentalną (Tracking Error)
    df['TrackingError'] = (df['MidPrice'] - df['Fundamental']).abs()

    # --- WYPISYWANIE ŚREDNICH (NOWE) ---
    print("\n" + "=" * 50)
    print("            RAPORT Z SYMULACJI")
    print("=" * 50)
    print(f"Liczba kroków symulacji: {len(df)}")
    print("-" * 50)
    print(f"SPREAD:")
    print(f"  Średni:        {df['Spread'].mean():.4f}")
    print(f"  Maksymalny:    {df['Spread'].max():.4f}")
    print(f"  Odchylenie Std:{df['Spread'].std():.4f}")
    print("-" * 50)
    print(f"RYZYKO I KOSZTY:")
    print(f"  Śr. Slippage:  {df['Slippage'].mean():.4f}")
    print(f"  Śr. Volatility:{df['Volatility'].mean():.4f}")
    print("-" * 50)
    print(f"EFEKTYWNOŚĆ RYNKU:")
    print(f"  Śr. Błąd Śledzenia (Tracking Error): {df['TrackingError'].mean():.4f}")
    print(f"  (Różnica między MidPrice a Fundamentem)")
    print("=" * 50 + "\n")

    # --- OBLICZANIE ŚREDNICH KROCZĄCYCH DO WYKRESÓW ---
    df['Spread_MA'] = df['Spread'].rolling(window=WINDOW_SIZE).mean()
    df['Slippage_MA'] = df['Slippage'].rolling(window=WINDOW_SIZE).mean()
    df['Volatility_MA'] = df['Volatility'].rolling(window=WINDOW_SIZE).mean()

    # Ustawiamy styl wykresów
    plt.style.use('ggplot')
    fig, axes = plt.subplots(3, 1, figsize=(15, 12), sharex=True)

    # 1. CENA (Price)
    axes[0].plot(df['Fundamental'], color='black', linestyle='--', linewidth=1.5, label='Fundamental (Input)')
    # Rysujemy 'szum' rynkowy (MidPrice)
    axes[0].plot(df['MidPrice'], color='blue', alpha=0.3, linewidth=0.5, label='Cena Rynkowa (MidPrice)')
    # Rysujemy wygładzony trend rynkowy
    axes[0].plot(df['MidPrice'].rolling(window=50).mean(), color='navy', linewidth=1.5, label='Trend Rynkowy (MA)')

    axes[0].set_title(f'Cena vs Fundament (Wygładzanie: {WINDOW_SIZE})')
    axes[0].set_ylabel('Cena')
    axes[0].legend(loc='upper left')

    # 2. SPREAD
    axes[1].plot(df['Spread'], color='gray', alpha=0.2, linewidth=0.5, label='Spread (Raw)')
    axes[1].plot(df['Spread_MA'], color='red', linewidth=2, label=f'Średni Spread ({WINDOW_SIZE} kroków)')

    axes[1].set_title('Płynność (Spread)')
    axes[1].set_ylabel('Spread')
    axes[1].legend(loc='upper left')

    # Opcjonalnie: Jeśli masz wielkie piki spreadu (flash crash), odkomentuj linię niżej, żeby przyciąć oś Y
    # axes[1].set_ylim(0, df['Spread'].quantile(0.99) * 2)

    # 3. SLIPPAGE & VOLATILITY
    axes[2].plot(df['Slippage_MA'], label=f'Śr. Slippage', color='orange', linewidth=2)
    axes[2].plot(df['Volatility_MA'], label=f'Śr. Volatility', color='green', linewidth=2)

    axes[2].set_title('Metryki Ryzyka (Wygładzone)')
    axes[2].set_ylabel('Wartość')
    axes[2].set_xlabel('Krok Symulacji')
    axes[2].legend(loc='upper left')

    plt.tight_layout()

    output_filename = "simulation_analysis.png"
    plt.savefig(output_filename, dpi=300)
    print(f"Zapisano wykres jako: {os.path.abspath(output_filename)}")

    plt.show()


if __name__ == "__main__":
    analyze()