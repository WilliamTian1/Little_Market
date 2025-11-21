import sys
import os
import random
import matplotlib.pyplot as plt

# Ensure the current directory is in the path
sys.path.append(os.getcwd())

# Add MinGW bin directory to DLL search path for Windows
if os.name == 'nt':
    try:
        os.add_dll_directory(r"C:\msys64\ucrt64\bin")
    except Exception:
        pass

try:
    import polysim
except ImportError as e:
    print(f"Failed to import polysim: {e}")
    sys.exit(1)

# Global Price Tracker
current_price = 100.0
price_history = []

class MarketMaker(polysim.Agent):
    def __init__(self, agent_id, cash, inventory):
        super().__init__(agent_id, cash, inventory)
        self.spread = 1.0
        self.order_qty = 10.0
        # Simple state to avoid flooding the book too much, though engine handles it fine
        self.last_tick_placed = -1

    def on_tick(self, market_snapshot):
        # Place quotes every tick to ensure liquidity is refreshed/maintained
        # In a real system, we'd check open orders. Here we just layer orders.
        # To keep it simple and avoid infinite book depth, we can rely on the fact 
        # that noise traders will eat them, or just place them.
        
        # Center around current price
        bid_price = current_price - self.spread
        ask_price = current_price + self.spread
        
        self.place_limit_order(polysim.Side.BUY, bid_price, self.order_qty)
        self.place_limit_order(polysim.Side.SELL, ask_price, self.order_qty)

    def on_trade(self, trade):
        global current_price
        current_price = trade.price
        # self.log(f"Trade executed at {trade.price}")

class NoiseTrader(polysim.Agent):
    def __init__(self, agent_id, cash, inventory):
        super().__init__(agent_id, cash, inventory)

    def on_tick(self, market_snapshot):
        # 10% chance to trade
        if random.random() < 0.1:
            if random.random() < 0.5:
                # Aggressive BUY
                self.place_limit_order(polysim.Side.BUY, 200.0, 1.0)
            else:
                # Aggressive SELL
                self.place_limit_order(polysim.Side.SELL, 1.0, 1.0)

    def on_trade(self, trade):
        global current_price
        current_price = trade.price

class Whale(polysim.Agent):
    def __init__(self, agent_id, cash, inventory):
        super().__init__(agent_id, cash, inventory)
        self.has_dumped = False

    def on_tick(self, market_snapshot):
        # Dump only once
        if not self.has_dumped:
            print(f"WHALE DUMPING AT TICK!")
            # Massive Sell Order to crash the price
            self.place_limit_order(polysim.Side.SELL, 13.0, 50000.0) 
            self.has_dumped = True

    def on_trade(self, trade):
        global current_price
        current_price = trade.price

def run_simulation():
    print("Initializing Simulation...")
    engine = polysim.Engine()

    # 1. Add Market Maker
    mm = MarketMaker(1, 100000.0, 1000.0)
    engine.add_agent(mm)

    # 2. Add Noise Traders
    noise_traders = []
    for i in range(50):
        nt = NoiseTrader(100 + i, 10000.0, 100.0)
        engine.add_agent(nt)
        noise_traders.append(nt)

    print("Running Simulation Loop...")
    whale_added = False
    
    for t in range(1000):
        # Introduce Whale at Tick 500
        if t == 500 and not whale_added:
            print("--- RELEASING THE WHALE ---")
            whale = Whale(999, 0.0, 10000.0)
            engine.add_agent(whale)
            whale_added = True

        # Run 1 tick
        engine.run(1)
        
        # Record Data
        price_history.append((t, current_price))
        
        if t % 100 == 0:
            print(f"Tick {t}: Price = {current_price:.2f}")

    print("Simulation Complete. Generating Plot...")
    
    # Visualization
    times = [x[0] for x in price_history]
    prices = [x[1] for x in price_history]

    plt.figure(figsize=(12, 6))
    plt.plot(times, prices, label='Market Price', color='blue', linewidth=1)
    plt.axvline(x=500, color='red', linestyle='--', label='Whale Dump')
    plt.title('PolySim: Flash Crash Simulation')
    plt.xlabel('Tick')
    plt.ylabel('Price')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    output_path = os.path.join(os.getcwd(), 'simulation_result.png')
    plt.savefig(output_path)
    print(f"Plot saved to {output_path}")

if __name__ == "__main__":
    run_simulation()
