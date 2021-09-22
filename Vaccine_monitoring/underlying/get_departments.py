import requests
from jsonpath import jsonpath

from underlying.set_global_variable import set_global_variable


def get_departments():
    # 创建新会话
    session = requests.session()

    # 获取全局变量
    globalVariable = set_global_variable()

    # 获取医院列表
    url = "https://mobile.scmttec.com/base/department/getDepartments.do"
    params = {
        "sortType": "1",
        "regionCode": globalVariable['region_code'],
        "offset": "0",
        "limit": "5",
        "isOpen": "1",
        "customId": "3",
        "longitude": globalVariable['longitude'],
        "latitude": globalVariable['latitude'],
    }

    # 获取响应值，将响应值转化为dict对象
    res = session.get(url=url, params=params)
    json_data = res.json()

    # 断言请求是否成功
    if res.status_code != 200:
        print("获取医院列表失败，请检查")
    else:
        # print("获取医院列表成功")
        pass

    # 通过jsonPath获取医院参数
    hospital_name = jsonpath(json_data, "$.data.rows..name")  # 获取医院Name列表
    depa_code = jsonpath(json_data, "$.data.rows..code")  # 获取医院Code列表
    hospital_depa_vacc_id = jsonpath(json_data, "$.data.rows..depaVaccId")  # 获取医院depaVaccId列表
    vaccine_code = jsonpath(json_data, "$.data.rows..vaccineCode")  # 获取疫苗Code列表

    return {
        'hospital_name': hospital_name,
        'depa_code': depa_code,
        'hospital_depa_vacc_id': hospital_depa_vacc_id,
        'vaccine_code': vaccine_code
    }


if __name__ == '__main__':
    get_departments()
