import re
import pandas as pd
import matplotlib.pyplot as plt
from collections import defaultdict

# 로그 파일들
log_files = ['ap42.txt']  # 여기에 원하는 로그 파일 이름 추가

# 정규 표현식
pattern = re.compile(
    r'(?P<time>\d+\.\d+)\s+IP\s+(?P<src>\d+\.\d+\.\d+\.\d+\.\d+)\s+>\s+(?P<dst>\d+\.\d+\.\d+\.\d+\.\d+):.*length\s+(?P<length>\d+)'
)

# flow 별로 데이터 저장
flows = defaultdict(list)

# 모든 파일에 대해 반복
for log_file in log_files:
    with open(log_file, 'r') as f:
        for line in f:
            match = pattern.search(line)
            if match:
                time = float(match.group('time'))
                src = match.group('src')
                dst = match.group('dst')
                length = int(match.group('length'))

                if dst == "10.1.3.1.8000":
                    flow = "Background"
                elif dst == "10.1.1.1.8002":
                    flow = "Flow2"
                elif dst == "10.1.3.1.8001":
                    flow = "Flow1"

                # flow = f"{src} -> {dst}"
                flows[flow].append((time, length))

# 그래프 그리기
for flow, packets in flows.items():
    df = pd.DataFrame(packets, columns=['time', 'length'])

    df['time_bin'] = (df['time'] * 10).astype(int) / 10  # 0.1초 단위
    # print(df['time_bin'])
    # print(df)

    grouped = df.groupby('time_bin')['length'].sum()
    # print(grouped)

    # Bytes per 0.1s를 Mbps로 변환 (8 bits = 1 byte)
    grouped_mbps = grouped * 8 / 1e6 / 0.1  # 0.1초 단위 처리 반영
    plt.plot(grouped.index, grouped_mbps.values, label=flow)

plt.xlabel('Time (s)')
plt.ylabel('Throughput (Mbps)')  # y축 레이블을 Mbps로 변경
plt.title('Scenario 3 Throughput')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
