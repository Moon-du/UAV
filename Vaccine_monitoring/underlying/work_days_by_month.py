import datetime

import requests
from jsonpath import jsonpath

from underlying.get_departments import get_departments
from underlying.set_global_variable import set_global_variable


def work_days_by_month():
    # 创建新会话
    session = requests.session()

    # 获取全局变量
    days = []
    global_variable = set_global_variable()

    # 获取医院基本信息
    hospital_info = get_departments()

    # 查看医院可预约日期
    i = 0  # 医院基本信息下标
    now_date = datetime.datetime.now().strftime('%Y-%m-%d')
    for hospital_depa_vacc_id in hospital_info['hospital_depa_vacc_id']:
        for vacc_index in global_variable['vacc_index']:
            url = "https://mobile.scmttec.com/order/subscribe/workDaysByMonth.do"
            params = {
                "vaccIndex": vacc_index,  # 可预约针次
                "vaccCode": hospital_info['vaccine_code'][i],
                "month": now_date,
                "linkmanId": global_variable['linkman_id'],
                "departmentVaccineId": hospital_depa_vacc_id,
                "depaCode": hospital_info['depa_code'][i]
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
                print("查询医院可预约时间失败，请检查")
            else:
                # print("查询医院可预约时间成功")
                pass

            # 通过jsonPath获取可预约日期并处理
            date_list = jsonpath(json_data, "$.data.dateList[*]")  # 获取可预约日期
            if not date_list:
                date_list = datetime.datetime.now().strftime('%Y%m%d')  # 如果可预约日期列表为空，设置缺省值为今天
            else:
                date_list = [str(k) for k in date_list]  # 列表元素转化为字符串
                date_list = ','.join(date_list)  # 列表转化为字符串
                date_list = date_list.strip('[]').replace('"', '').replace('-', '')  # 处理字符串特殊符号
            days.append(date_list)  # 将每次处理过的日期添加到列表

            # time.sleep(0.01)  # 设置等待，避免请求频繁返回False

        i += 1  # 医院基本信息下标每次+1

    return days


if __name__ == '__main__':
    work_days_by_month()
