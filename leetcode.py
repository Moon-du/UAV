from typing import Optional, List


class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next


class Leetcode:
    def addTwoNumbers(self, l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
        if not l1:
            return l2
        if not l2:
            return l1

        l1.val += l2.val  # 将两数相加，赋值给 l1 节点
        if l1.val >= 10:
            l1.next = self.addTwoNumbers(ListNode(l1.val // 10), l1.next)
            l1.val %= 10

        l1.next = self.addTwoNumbers(l1.next, l2.next)
        return l1

    def lengthOfLongestSubstring(self, s: str) -> int:
        # 哈希集合，记录每个字符是否出现过

        occ = set()
        n = len(s)
        # 右指针，初始值为 -1，相当于我们在字符串的左边界的左侧，还没有开始移动
        rk, ans = -1, 0
        for i in range(n):
            if i != 0:
                # 左指针向右移动一格，移除一个字符
                occ.remove(s[i - 1])
            while rk + 1 < n and s[rk + 1] not in occ:
                # 不断地移动右指针
                occ.add(s[rk + 1])
                rk += 1
            # 第 i 到 rk 个字符是一个极长的无重复字符子串
            ans = max(ans, rk - i + 1)
        print(ans)
        return ans

    def isPalindrome(self, x: int) -> bool:
        """回文数"""
        if x < 0:
            return False
        else:
            len_num = len(str(x)) // 2
            for i in range(len_num):
                if str(x)[i] != str(x)[-1 - i]:
                    return False
        return True

    def romanToInt(self, s: str) -> int:
        """最长公共前缀"""
        l = {
            "I": 1,
            "V": 5,
            "X": 10,
            "L": 50,
            "C": 100,
            "D": 500,
            "M": 1000,
        }
        sum = 0
        for i in range(len(s) - 1):
            if l[s[i]] >= l[s[i + 1]]:
                sum += l[s[i]]
            else:
                sum -= l[s[i]]
        sum += l[s[-1]]
        return sum

    def longestCommonPrefix(self, strs: List[str]) -> str:
        ans = []
        min_idx = 0
        for i in range(len(strs)):
            if len(strs[min_idx]) > len(strs[i]):
                min_idx = i
        min_str = strs[min_idx]
        if min_str == "":
            return ""
        else:
            for i in range(len(min_str)):
                l = []
                for str in strs:
                    l.append(str[i])
                if len(set(l)) > 1:
                    return "".join(ans)
                else:
                    ans.append(str[i])
            return "".join(ans)

    def isValid(self, s: str) -> bool:
        if len(s) % 2 == 1:
            return False

        r_map = {
            ")": "(",
            "]": "[",
            "}": "{",
        }
        new_list = list()
        for e in s:
            print(new_list)
            # 如果元素是右括号
            if e in r_map:
                # 如果新列表非空 或者新列表最后一个元素不是与当前括号e对应的右括号
                if not new_list or new_list[-1] != r_map[e]:
                    return False
                # 删除当前循环的元素
                new_list.pop()
            else:
                # 添加左括号
                new_list.append(e)

        print(new_list)
        return not new_list

    def removeDuplicates(self, nums: List[int]) -> int:
        """删除有序数组中的重复项"""
        k = 1
        if len(nums) == len(set(nums)):
            return len(nums)
        for i in range(len(nums) - 1):
            if nums[i] != nums[i + 1]:
                nums[k] = nums[i + 1]
                k += 1
            else:
                continue
        return len(set(nums))

    def removeElement(self, nums: List[int], val: int) -> int:
        """移除元素"""
        for i in range(len(nums)):
            if nums[i] == val:
                nums.append(nums[i])
                nums.remove(nums[i])
                print(nums)
        return len(nums) - nums.count(val)

    def searchInsert(self, nums: List[int], target: int) -> int:
        """搜索插入位置"""
        # if target in nums:
        #     return nums.index(target)
        # elif nums[-1] < target:
        #     nums.append(target)
        #     return len(nums) - 1
        # elif nums[0] > target:
        #     nums.insert(0, target)
        #     return 0
        # else:
        #     for i in range(len(nums) - 1):
        #         if nums[i] < target < nums[i + 1]:
        #             nums.insert(i + 1, target)
        #             return i + 1
        return (10 - 0) >> 1


if __name__ == "__main__":
    a = Leetcode().searchInsert([1, 3, 5, 7], 0)
    print(a)
