import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt

import pandas as pd

def plot_spread_in_time(book):
    plt.plot(book['seconds_in_bucket'], book['spread'])
    plt.title(f"Wykres: spread w czasie")
    plt.xlabel('Sekundy w bucket')
    plt.ylabel('Spread')
    plt.show()

def plot_midprice_in_time(book):
    plt.plot(book['seconds_in_bucket'], book['midprice'])
    plt.title(f"Wykres: midprice w czasie")
    plt.xlabel('Sekundy w bucket')
    plt.ylabel('Midprice')
    plt.show()

def scat_price_in_vol(trade):
    plt.scatter(trade['seconds_in_bucket'], trade['price'], s=trade['size'], alpha=0.5)
    plt.xlabel('Sekundy w bucket')
    plt.ylabel('Price')
    plt.title('Cena vs wolumen')
    plt.show()

def plot_avg_spread(spread):
    plt.plot(spread['seconds_in_bucket'], spread['spread'])
    plt.xlabel('Sekundy w bucket')
    plt.ylabel('Average spread')
    plt.show()

def plot_bid_vs_ask(bid, ask):
    plt.plot(bid['bid_orders'], bid['seconds_in_bucket'], label='Bid')
    plt.plot(ask['ask_orders'], bid['seconds_in_bucket'], label='Ask')
    plt.xlabel('Sekundy w bucket')
    plt.ylabel('Liczba zleceń')
    plt.title('Liczba zleceń po stronie Bid i Ask')
    plt.legend()
    plt.show()

def read_csv_from_cpp_sim(name):
    loc = f"../cpp_sim/{name}"
    df = pd.read_csv(loc)
    return df
