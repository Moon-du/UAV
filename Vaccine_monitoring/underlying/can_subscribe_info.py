from underlying.find_subscribe_amount_by_days import find_subscribe_amount_by_days


def can_subscribe_info():
    # 获取针次是否约满状态码列表
    is_max_list = find_subscribe_amount_by_days()

    # 获取可预约针次
    can_subscribe_vacc_index = []
    for vacc_index in range(len(is_max_list)):
        if 1 in is_max_list[vacc_index]:  # is_max_list[vacc_index]  = 1 代表当日可预约
            can_subscribe_vacc_index.append(vacc_index)

    # 根据针次获取可预约医院下标
    hospital_index = []
    for vacc_index in can_subscribe_vacc_index:
        hospital_index.append(int(vacc_index / 3))
    hospital_index = set(hospital_index)  # 对可预约医院下标去重
    hospital_index = list(hospital_index)  # 将去重后的集合转化为列表

    return hospital_index


if __name__ == '__main__':
    can_subscribe_info()
