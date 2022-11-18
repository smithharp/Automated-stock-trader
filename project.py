from pyrh import Robinhood
from pandas_datareader import data as pdr
from yahoo_fin import stock_info as si
from pandas import ExcelWriter
import matplotlib.pyplot as plt
import yfinance as yf
import pandas as pd
import requests
import datetime
import time

#Screens the entire NASDAQ for stocks that match our criteria
#Returns an array of ticker symbols that match our criteria
def averageAnalysis():
    stocklist = si.tickers_nasdaq()
    # data = yf.download("SPY AAPL GOOG", start="2021-12-06", end="2021-12-07", interval = "15m", group_by = 'ticker')
    # data.to_csv("TimeData.csv")

    n = -1
    # final = []
    df = pd.DataFrame();

    for stock in stocklist[0:40]:
        n += 1
        print ("\npulling {} with index {}".format(stock, n))
        currentStock = yf.Ticker(stock)
        mylist = []
        print ("downloading data...")
        stockData = currentStock.info
        print ("downloaded")

        try:
            print("Pulling 50-day...")
            fiftyd = stockData['fiftyDayAverage']
            print(fiftyd)
            print("Pulling 200-day...")
            twohundredd = stockData['twoHundredDayAverage']
            print(twohundredd)
            mylist = [stock, fiftyd, twohundredd]

        except Exception as e:
            print("Error: ")
            print(e)

        df = df.append([mylist])
    return(df)

def capm():
    stocklist = si.tickers_

    n = -1
    df = pd.DataFrame()

    for stock in stocklist[0:8]:
        n += 1
        print ("\npulling {} with index {}".format(stock, n))
        currentStock = yf.Ticker(stock)
        mylist = []
        print ("downloading data...")
        stockData = currentStock.info
        print ("downloaded")

        try:
            print("Pulling 52 week")
            fiftytwoweek = stockData['52WeekChange']
            print(fiftytwoweek)
            print("Pulling beta")
            beta = stockData['beta']
            print(beta)
            mylist = [stock, fiftytwoweek, beta]

        except Exception as e:
            print("Error: ")
            print(e)

        df = df.append([mylist])
    return(df)



def valueScreener():
    stocklist = si.tickers_nasdaq()
    df = pd.DataFrame()
    mylist = []

    for stock in stocklist[0:30]:
        print("Pulling " + stock + "...")
        try:
            val = si.get_stats_valuation(stock)
            val = val.iloc[:,:2]
            val.columns = ["Attribute", "Recent"]
            print(val)
            peRatio = float(val[val.Attribute.str.contains("Trailing P/E")].iloc[0,1])
            psRatio = float(val[val.Attribute.str.contains("Price/Sales")].iloc[0,1])
            pbRatio = float(val[val.Attribute.str.contains("Price/Book")].iloc[0,1])
            mylist = [stock, peRatio, psRatio, pbRatio]
        except Exception as e:
            print("Error: ")
            print(e)
        df = df.append([mylist])

    return(df)


#run the functions for pulling data
df = valueScreener()
df.to_csv('StockInfo2.csv', index=False, index_label=False)

df = averageAnalysis()
df.to_csv('Averages.csv', index=False, index_label=False)

df = capm()
df.to_csv('capm.csv', index=False, index_label=False)







