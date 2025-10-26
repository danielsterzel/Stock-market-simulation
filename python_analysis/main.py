import pandas as pd
import matplotlib.pyplot as plt

book = pd.read_parquet("../data/book/stock_id=0/c439ef22282f412ba39e9137a3fdabac.parquet").sample(50000)
trades = pd.read_parquet("../data/trade/stock_id=119/52f2e172b3444107903ec31242331923.parquet").sample(50000)

merged = pd.merge(trades, book, on=["time_id", "seconds_in_bucket"], how="inner")

merged["midprice"] = (merged["bid_price1"] + merged["ask_price1"]) / 2
merged["spread"] = merged["ask_price1"] - merged["bid_price1"]

# Głębokość rynku (top-2 poziomy)
merged["depth"] = merged["bid_size1"] + merged["ask_size1"] + merged["bid_size2"] + merged["ask_size2"]

print("Średni spread:", merged["spread"].mean())
print("Średnia głębokość (top2):", merged["depth"].mean())
print("Zmienność (std midprice):", merged["midprice"].std())
print("Średnia wielkość transakcji:", merged["size"].mean())

plt.figure(figsize=(10,4))
plt.scatter(merged["seconds_in_bucket"], merged["midprice"], color='blue', alpha=0.7, s=2)
plt.title("Midprice over time")
plt.xlabel("Seconds in bucket")
plt.ylabel("Midprice")
plt.show()

plt.figure(figsize=(10,4))
plt.scatter(merged["seconds_in_bucket"], merged["spread"], color='red', alpha=0.7, s=2)
plt.title("Spread over time")
plt.xlabel("Seconds in bucket")
plt.ylabel("Spread")
plt.show()

plt.figure(figsize=(10,4))
plt.hist(merged["spread"], bins=100, color='orange', alpha=0.8)
plt.title("Spread distribution")
plt.xlabel("Spread")
plt.ylabel("Count")
plt.show()


# zapis do csv żeby w c++ zrobić symualcje/model