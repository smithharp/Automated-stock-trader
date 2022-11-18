
Project: Stock data analysis
Authors: David Zhu and Harper Smith

Important note: This is not meant to be an actually functional program for trading stocks. As of the time of demonstration, December 2021, it does technically work. It can download information from YahooFinance and, supplied with account information on Robinhood, use that to make trades. However, the analysis of the stocks is rudimentary and due to errors with YahooFinance the data is incomplete. We strongly recommend not using this in real life. It is meant as a demonstration of the programming concepts you would need to create such a program.


The basic function of this program is as follows:
 - Launch a python script (project.py) which will pull stock data from yahoo finance and store it into 3 csv files
 - Create multiple threads to run analysis
	- Each thread will run one analysis on its respective CSV file and determine if it "likes" each stock in the list
	- If it "likes" the stock, it will increment one point to the stocks score on a global list
 - Make decisions based off of the analysis
	- If a stock has enough points on the global list, it is selected for a buy order
 - Launch a second python script which will execute a buy order for each stock which is favored by the analysis
 - Buy the stock using robinhood API


Three OS concepts (and an extra)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1. Processes
	We needed to use processes in order to launch the python script. This was the easiest option for interacting with yahoo finance since there are already established ways of doing it whereas we could not find anything on how to connect using C.

2. Parallelism with threads
	Ideally, we would be working with large amounts of real time data and so it could be important to run this analysis in a timely manner. With the small quantities of information we were using for testing, the threads don't really do anything to help but if we were to expand this program in the future it would be important to have them there.

3. Thread synchronization
	Since all the threads are updating a global list, we had to implement locks to ensure atomicity with manipulating the list. We also had to ensure that all the threads finish their analysis before the program makes any decisions about buying stocks.

4. Files and file systems
	The python script stores its pulled data in CSV files. We had to implement this on the python side as well as the code for C to read these files.




Additional notes
~~~~~~~~~~~~~~~~
Our current implementation of the code does NOT download new data into the csv files when run. There is a return 0 on line 317 which cuts off the child process for launching the first python script. This code does work, so if you remove that return statement it will launch the data pulling script. However, we experienced a number of server side issues with the yahoo finance platform so resorted to pre-downloaded data for testing and demonstration.
For unknown reasons it takes a long time to download the information, on the order of several seconds per stock. Additionally, the script sometimes crashes unpredictably. Our best guess is that yahoo finance decides at some point that it does not like our connection and decides to block it. Because of these factors as well as the large number of available stocks, we used our program to download small lists of tickers to run over for testing. Then we disabled the part of the code which pulls information at the start, so we don't have to wait 10 minutes each time and can reliably use a complete (for our purposes) data set. A future direction would be to find a way to quickly and accurately pull data for every stock on the market.


Conclusion
~~~~~~~~~~~
When you compile and run "launcher.c", it will create a new process for the python program. If you remove the return statement on line 317, it will launch the python script "project.py". If the return statement is still there, it will continue and use the csv files we have provided. It will then run the analysis, identify any stocks which meet the criteria, and launch the second python script "buy.py". This would execute the buy order. However, we have commented out the login so that it does not actually buy stocks in real life whenever the program is tested.