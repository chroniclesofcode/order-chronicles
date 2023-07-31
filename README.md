# order-chronicles
A fast limit order book for trade simulations and performance testing.

DONE:
- Created Matching Engine for orderbook
- Can read in LOBSTER csv data and parse into Orders
- Can process event types of LOBSTER entries properly
- RB + doubly linked lists for orderbook structure
- Can execute trades based on price-time priority
- Can populate the orderbook based on first entry in LOBSTER orderbook file
- Can print orderbook in LOBSTER format to compare to provided orderbook correctly
- Can execute trades based on LOBSTER trade specifications
- Pipes performance measurements into certain files
- Measure performance of the different data implementations
- Provide multiple runthroughs + statistics of each run
- Create array-based orderbook (array -> doubly linked lists)

TO DO:
- Optimize array-based orderbook to use it's advantages more
- Optimize parsing of orderbook
- Heavy optimizations (at time of writing, default mean = 152ms, arrays mean = 167ms)
- Create the orderbooks with/without STL
- Consider Van Emde Boas Tree (instead of RB) for LogLogN speed
- Update README for detailed information, including tradeoffs and results

FIXES: 
- Patch bug with full cancellation not working properly when orders have been executed on it
- Fix error where order type 4 matches improperly
- Process LOBSTER order book data properly without fail
- Revamp array-based orderbook due to price points being too large
- Fix error in arrays implementation where certain orders are not being added

# How to run

Currently, this orderbook works with command line arguments, and outputs to stdout. An example of a command may be
```
cmake .. && make && ./main "../assets/AAPL_msgs.csv" "../assets/AAPL_book.csv" > ../assets/AAPL_out.csv
```
For AAPL orderbook processing. This can be applied with other book/msgs if you change the filenames.
Note: this command MUST be run in the 'build' directory. If not, you may alter files outside of
the order-chronicles folder.

