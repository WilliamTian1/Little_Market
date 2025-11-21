import sys
import os
import random
import argparse
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
    def __init__(self, agent_id, cash, inventory, crunch_tick=None):
        super().__init__(agent_id, cash, inventory)
        self.spread = 1.0
        self.order_qty = 10.0
        self.crunch_tick = crunch_tick
        self.tick_count = 0

    def on_tick(self, market_snapshot):
        self.tick_count += 1
        
        # Liquidity Crunch Scenario: Stop quoting after a certain tick
        if self.crunch_tick and self.tick_count >= self.crunch_tick:
            return

        # Center around current price
        bid_price = current_price - self.spread
        ask_price = current_price + self.spread
        
        self.place_limit_order(polysim.Side.BUY, bid_price, self.order_qty)
        self.place_limit_order(polysim.Side.SELL, ask_price, self.order_qty)

    def on_trade(self, trade):
        global current_price
        current_price = trade.price

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
    def __init__(self, agent_id, cash, inventory, side, quantity):
        super().__init__(agent_id, cash, inventory)
        self.side = side
        self.quantity = quantity
        self.has_dumped = False

    def on_tick(self, market_snapshot):
        # Dump only once
        if not self.has_dumped:
            action = "BUYING" if self.side == polysim.Side.BUY else "SELLING"
            print(f"WHALE {action} {self.quantity} UNITS!")
            
            price = 200.0 if self.side == polysim.Side.BUY else 1.0
            self.place_limit_order(self.side, price, self.quantity) 
            self.has_dumped = True

    def on_trade(self, trade):
        global current_price
        current_price = trade.price

def run_simulation(args):
    print(f"Initializing Simulation: Scenario={args.scenario}")
    engine = polysim.Engine()

    # Reset Globals
    global current_price, price_history
    current_price = 100.0
    price_history = []

    # Keep references to agents to prevent GC
    agents = []

    # 1. Add Market Maker
    crunch_tick = args.whale_tick if args.scenario == 'liquidity_crunch' else None
    mm = MarketMaker(1, 1000000.0, 10000.0, crunch_tick=crunch_tick)
    engine.add_agent(mm)
    agents.append(mm)

    # 2. Add Noise Traders
    for i in range(args.agents):
        nt = NoiseTrader(100 + i, 10000.0, 100.0)
        engine.add_agent(nt)
        agents.append(nt)

    print("Running Simulation Loop...")
    event_triggered = False
    
    for t in range(args.ticks):
        # Trigger Event at specified tick
        if t == args.whale_tick and not event_triggered:
            if args.scenario == 'flash_crash':
                print("--- EVENT: FLASH CRASH (Whale Sell) ---")
                whale = Whale(999, 0.0, args.whale_qty, polysim.Side.SELL, args.whale_qty)
                engine.add_agent(whale)
                agents.append(whale)
                event_triggered = True
            elif args.scenario == 'rally':
                print("--- EVENT: MARKET RALLY (Whale Buy) ---")
                whale = Whale(999, args.whale_qty * 200, 0.0, polysim.Side.BUY, args.whale_qty)
                engine.add_agent(whale)
                agents.append(whale)
                event_triggered = True
            elif args.scenario == 'liquidity_crunch':
                print("--- EVENT: LIQUIDITY CRUNCH (MM Withdraws) ---")
                # MM logic handles this via crunch_tick
                event_triggered = True

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
    
    # Add vertical line for event
    plt.axvline(x=args.whale_tick, color='red', linestyle='--', label='Event Trigger')
    
    plt.title(f'PolySim: {args.scenario.replace("_", " ").title()}')
    plt.xlabel('Tick')
    plt.ylabel('Price')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    filename = f"result_{args.scenario}.png"
    output_path = os.path.join(os.getcwd(), filename)
    plt.savefig(output_path)
    print(f"Plot saved to {output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="PolySim Market Simulator")
    parser.add_argument("--scenario", type=str, default="flash_crash", 
                        choices=["flash_crash", "rally", "liquidity_crunch"],
                        help="Simulation scenario to run")
    parser.add_argument("--ticks", type=int, default=1000, help="Duration of simulation in ticks")
    parser.add_argument("--agents", type=int, default=50, help="Number of noise traders")
    parser.add_argument("--whale-qty", type=float, default=50000.0, help="Quantity for whale order")
    parser.add_argument("--whale-tick", type=int, default=500, help="Tick to trigger the event")

    args = parser.parse_args()
    run_simulation(args)
