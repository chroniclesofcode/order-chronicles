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
- Process LOBSTER order book data properly without fail
- Create array-based orderbook (array -> doubly linked lists)
- Create the orderbooks with/without STL
- Measure performance of the different data implementations
- Consider Van Emde Boas Tree (instead of RB) for LogLogN speed
- Update README for detailed information, including tradeoffs and results
