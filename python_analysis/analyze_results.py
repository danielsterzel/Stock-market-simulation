import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import os

import matplotlib

matplotlib.use('TkAgg')
plt.style.use('dark_background')

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUTPUT_DIR = os.path.join(BASE_DIR, "output")


class MultiPageAnalyzer:
    def __init__(self, df, name):
        self.df = df.copy()
        self.name = name
        self._calculate_all()

    def _calculate_all(self):
        df = self.df
        df['MidPrice'] = (df['BestBid'] + df['BestAsk']) / 2.0
        df['Returns'] = df['MidPrice'].pct_change()

        delta = df['MidPrice'].diff()
        gain = (delta.where(delta > 0, 0)).rolling(window=14).mean()
        loss = (-delta.where(delta < 0, 0)).rolling(window=14).mean()
        rs = gain / (loss + 1e-9)
        df['RSI'] = 100 - (100 / (1 + rs))

        exp1 = df['MidPrice'].ewm(span=12).mean()
        exp2 = df['MidPrice'].ewm(span=26).mean()
        df['MACD'] = exp1 - exp2
        df['Signal'] = df['MACD'].ewm(span=9).mean()

        roll_max = df['MidPrice'].cummax()
        df['Drawdown'] = (df['MidPrice'] - roll_max) / (roll_max + 1e-9)
        df['Vol_Rolling'] = df['Returns'].rolling(window=30).std()

        df['Spread_MA'] = df['Spread'].rolling(window=50).mean()
        df['Slippage_MA'] = df['Slippage'].rolling(window=50).mean()

        self.df = df.fillna(0)

    def show_page_1(self):
        fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(15, 12), sharex=True)
        fig.suptitle(f"PAGE 1/3: PRICE DYNAMICS - {self.name}", fontsize=16, color='cyan')

        ax1.plot(self.df['Fundamental'], color='#ff00ff', linestyle='--', alpha=0.5, label='Fundamental')
        ax1.plot(self.df['MidPrice'], color='#00ffff', linewidth=1.5, label='Market MidPrice')
        ax1.set_title("Price vs Fundamental Value")
        ax1.legend(loc='upper left')
        ax1.grid(alpha=0.1)

        ax2.plot(self.df['RSI'], color='#00ff00', linewidth=1)
        ax2.axhline(70, color='red', linestyle=':', alpha=0.6)
        ax2.axhline(30, color='red', linestyle=':', alpha=0.6)
        ax2.set_title("Momentum Indicator (RSI)")
        ax2.set_ylim(0, 100)
        ax2.grid(alpha=0.1)

        ax3.plot(self.df['MACD'], color='magenta', label='MACD')
        ax3.plot(self.df['Signal'], color='white', linestyle=':', label='Signal Line')
        ax3.bar(range(len(self.df)), self.df['MACD'] - self.df['Signal'], color='gray', alpha=0.3)
        ax3.set_title("Trend Strength (MACD)")
        ax3.legend(loc='upper left')
        ax3.grid(alpha=0.1)

        fig.subplots_adjust(top=0.92, bottom=0.08, hspace=0.3)
        plt.show(block=False)

    def show_page_2(self):
        fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(15, 12), sharex=True)
        fig.suptitle(f"PAGE 2/3: LIQUIDITY & EXECUTION - {self.name}", fontsize=16, color='yellow')

        ax1.fill_between(range(len(self.df)), self.df['Spread'], color='orange', alpha=0.2)
        ax1.plot(self.df['Spread_MA'], color='orange', linewidth=2, label='Avg Spread (MA50)')
        ax1.set_title("Bid-Ask Spread (Market Liquidity)")
        ax1.legend()
        ax1.grid(alpha=0.1)

        ax2.bar(range(len(self.df)), self.df['Slippage'], color='red', alpha=0.4, label='Raw Slippage')
        ax2.plot(self.df['Slippage_MA'], color='white', linewidth=1.5, label='Avg Slippage')
        ax2.set_title("Execution Costs (Slippage)")
        ax2.legend()
        ax2.grid(alpha=0.1)

        ax3.scatter(self.df['Spread'], self.df['Slippage'], color='cyan', alpha=0.2, s=10)
        ax3.set_title("Impact Analysis: Spread vs Slippage Correlation")
        ax3.set_xlabel("Spread Size")
        ax3.set_ylabel("Slippage Amount")
        ax3.grid(alpha=0.1)

        fig.subplots_adjust(top=0.92, bottom=0.08, hspace=0.3)
        plt.show(block=False)

    def show_page_3(self):
        fig = plt.figure(figsize=(15, 12))
        fig.suptitle(f"PAGE 3/3: RISK & DISTRIBUTION - {self.name}", fontsize=16, color='red')

        grid = plt.GridSpec(3, 2, hspace=0.4, wspace=0.3)

        ax1 = fig.add_subplot(grid[0, :])
        ax1.fill_between(range(len(self.df)), self.df['Drawdown'], 0, color='red', alpha=0.3)
        ax1.plot(self.df['Drawdown'], color='red', linewidth=1)
        ax1.set_title("Maximum Drawdown (Underwater Chart)")
        ax1.grid(alpha=0.1)

        ax2 = fig.add_subplot(grid[1, 0])
        sns.histplot(self.df['Returns'], kde=True, ax=ax2, color='cyan', bins=50)
        ax2.set_title("Returns Distribution")

        ax3 = fig.add_subplot(grid[1, 1])
        ax3.plot(self.df['Vol_Rolling'], color='yellow', linewidth=1.5)
        ax3.set_title("Rolling Volatility (30 steps)")
        ax3.grid(alpha=0.1)

        ax4 = fig.add_subplot(grid[2, :])
        cum_ret = (1 + self.df['Returns']).cumprod() - 1
        ax4.plot(cum_ret, color='lime', linewidth=2)
        ax4.fill_between(range(len(self.df)), cum_ret, 0, color='green', alpha=0.1)
        ax4.set_title("Cumulative Market Return")
        ax4.grid(alpha=0.1)

        # Używamy subplots_adjust zamiast tight_layout dla GridSpec
        fig.subplots_adjust(top=0.92, bottom=0.08, left=0.1, right=0.9)
        plt.show()


def main():
    if not os.path.exists(OUTPUT_DIR):
        return

    files = [f for f in os.listdir(OUTPUT_DIR) if f.endswith('.csv')]
    if not files:
        return

    latest_file = max([os.path.join(OUTPUT_DIR, f) for f in files], key=os.path.getctime)
    df = pd.read_csv(latest_file)

    analyzer = MultiPageAnalyzer(df, os.path.basename(latest_file))
    analyzer.show_page_1()
    analyzer.show_page_2()
    analyzer.show_page_3()


if __name__ == "__main__":
    main()