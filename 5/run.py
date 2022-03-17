import subprocess
import time

subprocess.Popen(['wine', 'bin/main.exe', 'w1', 'w2'])
time.sleep(2)
subprocess.Popen(['wine', 'bin/main.exe', 'w2', 'w1'])
