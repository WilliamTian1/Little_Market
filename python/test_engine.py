import sys
import os

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

class BuyerBot(polysim.Agent):
    def __init__(self, agent_id, cash, inventory):
        super().__init__(agent_id, cash, inventory)
        self.trade_count = 0

    def on_tick(self, market_snapshot):
        # Place a Buy Order @ 100
        print(f"Buyer {self.agent_id} placing Buy Order @ 100")
        self.place_limit_order(polysim.Side.BUY, 100.0, 10.0)

    def on_trade(self, trade):
        print(f"Buyer {self.agent_id} executed trade: {trade.quantity} @ {trade.price}")
        self.trade_count += 1

class SellerBot(polysim.Agent):
    def __init__(self, agent_id, cash, inventory):
        super().__init__(agent_id, cash, inventory)
        self.trade_count = 0

    def on_tick(self, market_snapshot):
        # Place a Sell Order @ 100
        print(f"Seller {self.agent_id} placing Sell Order @ 100")
        self.place_limit_order(polysim.Side.SELL, 100.0, 10.0)

    def on_trade(self, trade):
        print(f"Seller {self.agent_id} executed trade: {trade.quantity} @ {trade.price}")
        self.trade_count += 1

def test_engine():
    print("Testing Simulation Engine...")
    engine = polysim.Engine()

    buyer = BuyerBot(1, 10000.0, 0.0)
    seller = SellerBot(2, 0.0, 100.0)

    engine.add_agent(buyer)
    engine.add_agent(seller)

    print("Running engine for 1 tick...")
    engine.run(1)

    # Assertions
    assert buyer.trade_count > 0, "Buyer did not trade!"
    assert seller.trade_count > 0, "Seller did not trade!"
    
    print("Engine test passed! Trades executed.")

if __name__ == "__main__":
    test_engine()
