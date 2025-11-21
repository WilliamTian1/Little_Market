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

class MyBot(polysim.Agent):
    def __init__(self, agent_id, cash, inventory):
        super().__init__(agent_id, cash, inventory)
        self.tick_received = False

    def on_tick(self, market_snapshot):
        print(f"Python Agent {self.agent_id} received tick! Snapshot: {market_snapshot}")
        self.tick_received = True

    def on_trade(self, trade):
        print(f"Python Agent {self.agent_id} received trade: {trade.price} @ {trade.quantity}")

def test_agent_inheritance():
    print("Testing Agent inheritance...")
    bot = MyBot(1, 10000.0, 50.0)
    
    # Verify initial state
    assert bot.agent_id == 1
    assert bot.cash == 10000.0
    assert bot.inventory == 50.0
    print("Initial state verified.")

    # Pass to C++ to trigger callback
    print("Passing agent to C++...")
    polysim.test_agent_callback(bot)
    
    # Verify callback was received
    assert bot.tick_received, "on_tick was not called!"
    print("Callback verified!")

if __name__ == "__main__":
    test_agent_inheritance()
