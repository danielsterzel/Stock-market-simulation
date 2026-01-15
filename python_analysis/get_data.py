import yfinance as yf
import pandas as pd
import numpy as np
import os
import sys

# --- KONFIGURACJA ---
OUTPUT_DIR = "../data"
STEPS_TOTAL = 6500

# ZESTAW SCENARIUSZY
STOCKS_CONFIG = [
    {
        "name": "input_bubble.csv",
        "ticker": "GME",  # GameStop
        "start": "2021-01-01",
        "end": "2021-03-01",  # Szczyt bańki Redditowej
        "desc": "Bańka Spekulacyjna (GME 2021)"
    },
    {
        "name": "input_crisis.csv",
        "ticker": "SPY",  # S&P 500
        "start": "2008-09-01",
        "end": "2008-12-01",  # Upadek Lehman Brothers
        "desc": "Wielki Kryzys Finansowy (2008)"
    },
    {
        "name": "input_stable.csv",
        "ticker": "KO",  # Coca-Cola
        "start": "2019-01-01",
        "end": "2019-06-01",  # Bardzo nudny, stabilny okres
        "desc": "Rynek Stabilny (Coca-Cola 2019)"
    },
    {
        "name": "input_growth.csv",
        "ticker": "NVDA",  # Nvidia
        "start": "2023-01-01",
        "end": "2023-06-01",  # Hossa AI (silny trend wzrostowy)
        "desc": "Silny Trend Wzrostowy (AI Boom)"
    }
]


def ensure_dir():
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)


def process_real_data():
    ensure_dir()
    print("--- POBIERANIE PRAWDZIWYCH DANYCH HISTORYCZNYCH ---")

    for item in STOCKS_CONFIG:
        print(f"[{item['ticker']}] Pobieranie: {item['desc']}...")
        try:
            df = yf.download(item['ticker'], start=item['start'], end=item['end'], progress=False)

            if df.empty:
                print(f"   [ERROR] Brak danych dla {item['ticker']}!")
                continue

            prices = df['Close'].values.flatten()

            daily_count = len(prices)
            steps_per_day = int(STEPS_TOTAL / daily_count)

            smooth_prices = []
            for i in range(daily_count - 1):
                start_p = prices[i]
                end_p = prices[i + 1]
                chunk = np.linspace(start_p, end_p, steps_per_day)
                smooth_prices.extend(chunk)

            final_series = np.array(smooth_prices)
            start_val = final_series[0]
            final_series = (final_series / start_val) * 100.0

            path = os.path.join(OUTPUT_DIR, item['name'])
            pd.DataFrame(final_series).to_csv(path, header=False, index=False)
            print(f"   [OK] Zapisano: {item['name']} ({len(final_series)} kroków)")

        except Exception as e:
            print(f"   [CRITICAL] Błąd przy {item['ticker']}: {e}")


def generate_synthetic():
    print("\n--- GENEROWANIE DANYCH SYNTETYCZNYCH ---")
    # Sinusoida z szumem
    prices = []
    x = np.linspace(0, 6 * np.pi, STEPS_TOTAL)
    for i in x:
        val = 100.0 + 15 * np.sin(i) + np.random.normal(0, 0.1)
        prices.append(val)

    path = os.path.join(OUTPUT_DIR, "input_sine.csv")
    pd.DataFrame(prices).to_csv(path, header=False, index=False)
    print(f"   [OK] Zapisano: input_sine.csv (Sinusoida)")


if __name__ == "__main__":
    process_real_data()
    generate_synthetic()
    print("\nGOTOWE! Wszystkie pliki są w folderze ../data/")