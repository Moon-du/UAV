import requests
from jsonpath import jsonpath

from underlying.get_departments import get_departments
from underlying.set_global_variable import set_global_variable


def is_can_subscribe():
    # 创建新会话
    session = requests.session()

    # 获取全局变量
    type_code_list = []
    global_variable = set_global_variable()

    # 获取医院基本信息
    hospital_info = get_departments()

    # 获取医院详情
    i = 0  # 医院基本信息下标
    for hospital_depa_vacc_id in hospital_info['hospital_depa_vacc_id']:
        url = "https://mobile.scmttec.com/subscribe/subscribe/isCanSubscribe.do"
        params = {
            "vaccineCode": hospital_info['vaccine_code'][i],
            "linkmanId": global_variable['linkman_id'],
            "id": hospital_depa_vacc_id,
            "depaCode": hospital_info['depa_code'][i]
        }
        headers = {
            'tk': global_variable['tk'],
            'cookie': global_variable['cookie']
        }
        i += 1  # 医院基本信息下标每次+1
        # 获取响应值，将响应值转化为dict对象
        res = session.get(url=url, params=params, headers=headers)
        json_data = res.json()

        # 断言请求是否成功
        if res.status_code != 200:
            print("查询医院状态失败，请检查")
        else:
            # print("查询医院状态成功")
            pass

        # 通过jsonPath获取状态
        typeCode = jsonpath(json_data, "$.data.typeCode")  # 获取医院状态
        typeCode = [str(i) for i in typeCode]  # 列表元素转化为字符串
        typeCode = int(','.join(typeCode))  # 列表元素转化为整形拼接
        type_code_list.append(typeCode)

    return type_code_list


if __name__ == '__main__':
    is_can_subscribe()
