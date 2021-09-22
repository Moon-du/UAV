import time

import requests
from jsonpath import jsonpath

from underlying.get_departments import get_departments
from underlying.set_global_variable import set_global_variable
from underlying.work_days_by_month import work_days_by_month


def find_subscribe_amount_by_days():
    # 创建新会话
    session = requests.session()

    # 获取全局变量
    is_max_list = []
    global_variable = set_global_variable()

    # 获取医院基本信息
    hospital_info = get_departments()

    # 获取可预约日期
    days = work_days_by_month()

    # 查看可预约日期是否约满

    i = 0  # 医院基本信息下标
    j = 0  # 可订阅日期下标

    for hospital_depa_vacc_id in hospital_info['hospital_depa_vacc_id']:
        for vacc_index in global_variable['vacc_index']:
            url = "https://mobile.scmttec.com/subscribe/subscribe/findSubscribeAmountByDays.do"
            params = {
                "vaccIndex": vacc_index,
                "vaccCode": hospital_info['vaccine_code'][i],
                "linkmanId": global_variable['linkman_id'],
                "departmentVaccineId": hospital_depa_vacc_id,
                "depaCode": hospital_info['depa_code'][i],
                "days": days[j]
            }
            headers = {
                'tk': global_variable['tk'],
                'cookie': global_variable['cookie']
            }

            # 获取响应值，将响应值转化为dict对象
            res = session.get(url=url, params=params, headers=headers)
            json_data = res.json()

            # 断言请求是否成功
            if res.status_code != 200:
                print("查询医院可预约日期是否约满失败，请检查")
            else:
                # print("查询医院可预约日期是否约满成功")
                pass

            # 通过jsonPath获取日期是否约满状态码列表
            max_sub_list = jsonpath(json_data, "$..maxSub")  # 获取日期是否约满状态码列表
            if not max_sub_list:
                max_sub_list = 0  # 如果可预约日期列表为空，设置缺省值为0
            else:
                pass
            is_max_list.append(max_sub_list)  # 将状态码添加到列表

            j += 1  # 可订阅日期下标每次+1

            time.sleep(0.02)  # 设置等待，避免请求频繁返回False
        i += 1  # 医院基本信息下标每次+1

    return is_max_list


if __name__ == '__main__':
    find_subscribe_amount_by_days()
