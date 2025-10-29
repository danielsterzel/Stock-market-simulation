import pandas    as pd
import functions as fun

book_stock_id_0 = pd.read_parquet("../data/book/stock_id=0/c439ef22282f412ba39e9137a3fdabac.parquet").sample(10000)
book_stock_id_1 = pd.read_parquet("../data/book/stock_id=1/31a1c5cd6d8546b383d10373db762236.parquet").sample(10000)

trade_stock_id_119 = pd.read_parquet("../data/trade/stock_id=119/52f2e172b3444107903ec31242331923.parquet").sample(10000)
trade_stock_id_120 = pd.read_parquet("../data/trade/stock_id=120/0d06cf1025d84e19be32c9d9ffed9fd2.parquet").sample(10000)
trade_stock_id_122 = pd.read_parquet("../data/trade/stock_id=122/a2e2aa41640f4986bebc1b95bd29966f.parquet").sample(10000)

book_stocks = [book_stock_id_0, book_stock_id_1]
trade_stocks = [trade_stock_id_119, trade_stock_id_120, trade_stock_id_122]

for book in book_stocks:
    book['spread'] = book['ask_price1'] - book['bid_price1']
    book['midprice'] = (book['bid_price1'] + book['ask_price1']) / 2
    book['bid_orders'] = book['bid_size1'] + book['bid_size2']
    book['ask_orders'] = book['ask_size1'] + book['ask_size2']

for trade in trade_stocks:
    trade['vwap'] = (trade['price'] * trade['size']).cumsum() / trade['size'].cumsum()

fun.plot_spread_in_time(book_stock_id_0)
fun.plot_spread_in_time(book_stock_id_1)

fun.plot_midprice_in_time(book_stock_id_0)
fun.plot_midprice_in_time(book_stock_id_1)

fun.scat_price_in_vol(trade_stock_id_119)
fun.scat_price_in_vol(trade_stock_id_120)
fun.scat_price_in_vol(trade_stock_id_122)

avg_spread = []
for book in book_stocks:
    spread = book.groupby('seconds_in_bucket')['spread'].mean().reset_index()
    avg_spread.append(spread)

fun.plot_avg_spread(avg_spread[0])
fun.plot_avg_spread(avg_spread[1])

bid_sums = []
ask_sums = []
for book in book_stocks:
    bid_sum = book.groupby('seconds_in_bucket')['bid_orders'].sum().reset_index()
    ask_sum = book.groupby('seconds_in_bucket')['ask_orders'].sum().reset_index()
    bid_sums.append(bid_sum)
    ask_sums.append(ask_sum)

fun.plot_bid_vs_ask(bid_sums[0], ask_sums[0])
fun.plot_bid_vs_ask(bid_sums[1], ask_sums[1])

# Zapis średniego spreadu
avg_spread[0].to_csv("avg_spread_stock_0.csv", index=False)
avg_spread[1].to_csv("avg_spread_stock_1.csv", index=False)

# Zapis sumy bid/ask
bid_sums[0].to_csv("bid_orders_stock_0.csv", index=False)
ask_sums[0].to_csv("ask_orders_stock_0.csv", index=False)
bid_sums[1].to_csv("bid_orders_stock_1.csv", index=False)
ask_sums[1].to_csv("ask_orders_stock_1.csv", index=False)

# VWAP z transakcji
trade_stocks[0].to_csv("trade_stock_119.csv", index=False)
trade_stocks[1].to_csv("trade_stock_120.csv", index=False)
trade_stocks[2].to_csv("trade_stock_122.csv", index=False)