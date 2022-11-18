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
import sys


#QR = ""
#rh = Robinhood()
#rh.login(username="", password="", qr_code=QR)
#User information removed for the sake of privacy
stock = sys.argv[1]
print("ticker: " + stock)
instrument = rh.instrument(stock)
rh.place_buy_order(instrument, 1, ask_price=1.0)






