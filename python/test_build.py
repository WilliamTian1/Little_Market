import sys
import os

# Ensure the current directory is in the path so we can find the compiled module
sys.path.append(os.getcwd())

# Add MinGW bin directory to DLL search path for Windows
if os.name == 'nt':
    try:
        os.add_dll_directory(r"C:\msys64\ucrt64\bin")
    except Exception:
        pass

try:
    import polysim
    print(f"Successfully imported polysim module: {polysim}")
except ImportError as e:
    print(f"Failed to import polysim: {e}")
    sys.exit(1)

def test_add():
    result = polysim.add(1, 2)
    print(f"polysim.add(1, 2) = {result}")
    assert result == 3, f"Expected 3, got {result}"
    print("Test passed!")

if __name__ == "__main__":
    test_add()
