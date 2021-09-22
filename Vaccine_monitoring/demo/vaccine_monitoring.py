import time

from underlying.can_subscribe_info import can_subscribe_info
from underlying.get_departments import get_departments
from underlying.is_can_subscribe import is_can_subscribe
from underlying.send_notification import send_notification


def vaccine_monitoring():
    send_type = 0  # 通知状态设置为未通知
    rounds = 1  # 运行次数

    while True:
        start = time.time()

        # 获取医院基本信息
        hospital_info = get_departments()

        # 获取医院状态查看医院是否可预约
        type_code_list = is_can_subscribe()

        type_code_index = 0  # 状态码下标为0
        for type_code in type_code_list:  # 状态码1为可预约

            if type_code == 1:
                print(f'{hospital_info["hospital_name"][type_code_index]}可预约')

                # 获取可预约医院下标列表
                hospital_index = can_subscribe_info()

                # 是否发送通知（列表不为空并且状态为缺货时进入循环）
                if len(hospital_index) != 0 and send_type == 0:
                    i = 0  # 医院下标
                    for hospital in hospital_index:
                        if hospital == i:
                            # 通过钉钉机器人发送医院i到苗信息
                            send_notification(hospital_info["hospital_name"][i])
                            i += 1  # 医院下标每次+1
                            send_type = 1  # 通知状态设置为已通知
                        else:
                            pass

                else:
                    if len(hospital_index) == 0:
                        send_type = 0  # 通知状态设置为未通知
                    else:
                        pass

            elif type_code == 2:
                print(f'{hospital_info["hospital_name"][type_code_index]}可订阅')

            elif type_code == 3:
                print(f'{hospital_info["hospital_name"][type_code_index]}已订阅等待到苗通知')

            elif type_code == 4:
                print(f'{hospital_info["hospital_name"][type_code_index]}暂停订阅')

            else:
                pass

            type_code_index += 1  # 状态码下标每次+1

        rounds += 1  # 运行次数每次+1

        end = time.time()
        print(f"_______________________已运行{rounds}次,本次运行时间{format(end - start)}秒__________________________________\n\n")


if __name__ == '__main__':
    vaccine_monitoring()
