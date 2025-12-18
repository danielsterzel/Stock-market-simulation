import yfinance as yf
import pandas as pd
import numpy as np
import os

TICKER = "AAPL"
START = "2023-01-01"
END = "2023-06-01"
STEPS_PER_DAY = 50
OUTPUT_FILE = "../data/input_prices.csv"


def generate():
    print(f"Pobieranie danych dla {TICKER}...")
    data = yf.download(TICKER, start=START, end=END)

    if len(data) == 0:
        print("Błąd: Nie pobrano danych. Sprawdź połączenie lub daty.")
        return

    daily_prices = data['Close'].values.flatten() 

    print(f"Pobrano {len(daily_prices)} dni handlowych.")

    smooth_prices = []

    for i in range(len(daily_prices) - 1):
        start_price = daily_prices[i]
        end_price = daily_prices[i + 1]

        linspace = np.linspace(start_price, end_price, STEPS_PER_DAY)
        smooth_prices.extend(linspace)

    df = pd.DataFrame(smooth_prices)
    df.to_csv(OUTPUT_FILE, header=False, index=False)

    print(f"\nSUKCES! Wygenerowano plik: {OUTPUT_FILE}")
    print(f"Liczba kroków dla symulacji: {len(smooth_prices)}")
    print(f"Ścieżka absolutna: {os.path.abspath(OUTPUT_FILE)}")
    print("Skopiuj tę ścieżkę do swojego main.cpp w C++!")


if __name__ == "__main__":
    generate()