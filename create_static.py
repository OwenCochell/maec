"""
Python Program that makes a proper static chain    
"""

# Number of middle modules in the chain
NUM = 200

# Source in the static chain
SOURCE = "ConstModule"

# Sink in the static chain
SINK = "SinkModule"

# Middle module in the static chain
MID = "AmplitudeScale"

# Create the initial string:

init = f"{SINK}<"

# Add the middle modules

init += f"{MID}<" * NUM

# Add the source

init += SOURCE

# Add the template brackets

init += ">" * (NUM + 1)

print(init + "sink;")
