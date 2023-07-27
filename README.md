# order-chronicles
A fast limit order book for trade simulations.

DONE:
- Created Matching Engine for orderbook
- Can read in LOBSTER csv data and parse into Orders
- Can process event types of LOBSTER entries properly
- RB + doubly linked lists for orderbook structure
- Can execute trades based on price-time priority
- Can populate the orderbook based on first entry in LOBSTER orderbook file

TO DO:
- Patch bug with full cancellation not working properly when orders have been executed on it
- Add in error detection if an order of event type 4 matches improperly
- Process LOBSTER order book data properly without fail
- Measure performance of the different data implementations
- Create array-based orderbook (array -> doubly linked lists)
- Create the orderbooks with/without STL
- Consider Van Emde Boas Tree (instead of RB) for LogLogN speed
- Update README for detailed information, including tradeoffs and results

# How to run

This orderbook works with command line arguments, and outputs to stdout. An example of a command may be
```
cmake .. && make && ./main "../assets/AAPL_msgs.csv" "../assets/AAPL_book.csv" > ../assets/AAPL_out.csv
```
For AAPL orderbook processing. This can be applied with other book/msgs if you change the filenames.

