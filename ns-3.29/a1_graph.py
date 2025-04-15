import matplotlib.pyplot as plt
from collections import defaultdict

# 파일 경로 설정
log_file = './log_a1.dat'

# flow별 데이터를 저장할 딕셔너리 초기화
flow_data = defaultdict(lambda: {'time': [], 'value': []})

# 파일 읽기 및 파싱
with open(log_file, 'r') as file:
    for line in file:
        parts = line.strip().split()
        if len(parts) != 3:
            continue  # 잘못된 라인 건너뛰기
        flow_id = int(parts[0])
        time = float(parts[1])
        throughput = float(parts[2])
        flow_data[flow_id]['time'].append(time)
        flow_data[flow_id]['value'].append(throughput)

# 그래프 그리기
plt.figure(figsize=(10, 5))

# flow별로 그래프 그리기
for flow_id, data in flow_data.items():
    if flow_id == 0:
        plt.plot(data['time'], data['value'], label=f'Background',  linestyle='-')
    else:
        plt.plot(data['time'], data['value'], label=f'Flow{flow_id}',  linestyle='-')

# 제목 및 축 레이블
plt.title('Scenario 3 Throughput')
plt.xlabel('Time (s)')
plt.ylabel('Throughput (Mbps)')
plt.legend()
plt.grid(True)

# 저장 or 표시
plt.savefig('./scenario3_throughput.png')  # 저장 원하면 주석 해제
plt.show()


# import re
# import pandas as pd
# import matplotlib.pyplot as plt
# from collections import defaultdict

# # 로그 파일들
# log_files = ['a1_to0.txt', 'a1_to1.txt']  # 여기에 원하는 로그 파일 이름 추가

# # 정규 표현식
# pattern = re.compile(
#     r'(?P<time>\d+\.\d+)\s+IP\s+(?P<src>\d+\.\d+\.\d+\.\d+\.\d+)\s+>\s+(?P<dst>\d+\.\d+\.\d+\.\d+\.\d+):.*length\s+(?P<length>\d+)'
# )

# # flow 별로 데이터 저장
# flows = defaultdict(list)

# # 모든 파일에 대해 반복
# for log_file in log_files:
#     with open(log_file, 'r') as f:
#         for line in f:
#             match = pattern.search(line)

#             if match:

#                 if log_file == "a1_to0.txt":
#                     src = match.group('src')
#                     if src == "10.1.1.1.49153":
#                         continue


#                 time = float(match.group('time'))
#                 src = match.group('src')
#                 dst = match.group('dst')
#                 length = int(match.group('length'))

#                 if dst == "10.1.3.1.8000":
#                     flow = "Background"
#                 elif dst == "10.1.1.1.8002":
#                     flow = "Flow2"
#                 elif dst == "10.1.3.1.8001":
#                     flow = "Flow1"

#                 # flow = f"{src} -> {dst}"
#                 flows[flow].append((time, length))

# # 그래프 그리기
# for flow, packets in flows.items():
#     df = pd.DataFrame(packets, columns=['time', 'length'])

#     df['time_bin'] = (df['time'] * 10).astype(int) / 10  # 0.1초 단위
#     # print(df['time_bin'])
#     # print(df)

#     grouped = df.groupby('time_bin')['length'].sum()
#     # print(grouped)

#     # Bytes per 0.1s를 Mbps로 변환 (8 bits = 1 byte)
#     grouped_mbps = grouped * 8 / 1e6 / 0.1  # 0.1초 단위 처리 반영
#     plt.plot(grouped.index, grouped_mbps.values, label=flow)

# plt.xlabel('Time (s)')
# plt.ylabel('Throughput (Mbps)')  # y축 레이블을 Mbps로 변경
# plt.title('Scenario 3 Throughput')
# plt.legend()
# plt.grid(True)
# plt.tight_layout()
# plt.show()
