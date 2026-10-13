#!/usr/bin/env python3
"""Generate the deterministic, synthetic million-event teaching sample."""
import random

rng = random.Random(20261012)
N = 1_000_000
with open("muons_raw.txt", "w", encoding="utf-8") as f:
    f.write("# Synthetic teaching data. Times in ns; ADC channels; quality=1 passes basic acquisition flag.\n")
    f.write("# event_id t_start_ns t_stop_ns q_start_adc q_stop_adc quality\n")
    for event_id in range(N):
        t_start_ns = 10_000_000 + event_id * 50_000 + rng.randrange(1000)
        dt_ns = max(1, round(rng.expovariate(1 / (2.197 * 1000))))
        t_stop_ns = t_start_ns + dt_ns
        q_start_adc = min(250, max(1, round(rng.gauss(100, 9))))
        q_stop_adc = min(250, max(1, round(0.88 * q_start_adc + 22 + rng.gauss(0, 12))))
        quality = int(rng.random() > 0.01)
        f.write(f"{event_id} {t_start_ns} {t_stop_ns} {q_start_adc} {q_stop_adc} {quality}\n")
print(f"Wrote {N} synthetic events to muons_raw.txt")
