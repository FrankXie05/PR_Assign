from ast import In
from unittest import result
from urllib import response
import requests
import json
import subprocess
import os
import queue
import threading
import csv
import pandas as pd

def input_with_timeout(prompt, timeout, default):
    result_queue = queue.Queue()

    def input_thread():
        try:
            result = input(prompt)
            result_queue.put(result)
        except Exception as e:
            result_queue.put(default)

    thread = threading.Thread(target=input_thread)
    thread.daemon = True
    thread.start()

    thread.join(timeout)

    if result_queue.qsize() > 0:
        return result_queue.get()
    else:
        print("使用默认值")
        return default

def get_open_prs(url, headers, count):
    params = {"per_page": count}
    try:
        response = requests.get(url, headers=headers, params=params)
        response.raise_for_status()
        return response.json()   
    except requests.RequestException as e:
        print(f"Failed to retrieve PRs. Status code: {response.status_code}")
        return None

def get_prs(url,headers,count):
    params = {"per_page": count,"state": "all"}
    try:
        response = requests.get(url, headers=headers, params = params)
        response.raise_for_status()
        return response.json()
    except requests.RequestException as e:
        print(f"获取最新的 {count} PR. 错误代码：{response.status_code}")
        return None

def get_pr_info(url, headers, pr_number):
    pr_re_info = pr_number
    response = requests.get(f"{url}/{pr_re_info}", headers=headers)
    
    if response.status_code == 200:
        pr_re_info = response.json()
        
        if pr_re_info.get("state") == "open":
            return pr_re_info
        else:
            return None
    elif response.status_code == 404:
        print(f"pr: {pr_number} 不存在")
        return None
    else:
        print(f" 无法找到pr: {pr_number}, 错误代码：{response.status_code}")
        return None

def get_assigenn_from_pr_info(pr_info):
    if pr_info:
        currect_pr_login = pr_info["assignee"]["login"]
        return currect_pr_login
    else:
        return None


def del_login_by_cti(sublist_org, CTI_User_list):
    indexes_to_remove = []

    for i, sublist in enumerate(sublist_org):
        user_info = sublist[0]
        login = user_info["user"]["login"]
        
        if login in CTI_User_list:
            indexes_to_remove.append(i)

    for index in reversed(indexes_to_remove):
        del sublist_org[index]

    return sublist_org

def split_numbers_by_assignee(list_org):
    numbers_with_assignee = []
    numbers_without_assignee = []

    for list in list_org:
        list_info = list[0]
        assignee = list_info["assignee"]
        number = list_info["number"]
        state = list_info["state"]

        if assignee is not None:
            numbers_with_assignee.append(number)
            
        else:
            if state == 'open':
                numbers_without_assignee.append(number)
            
        numbers_with_assignee_sorted = sorted(numbers_with_assignee, reverse=True)
        numbers_without_assignee_sorted = sorted(numbers_without_assignee)
    
    return numbers_with_assignee_sorted, numbers_without_assignee_sorted

def compare_numbers_loop(numbers_with_assignee, numbers_without_assignee):
    for number_assignee in numbers_with_assignee:
        
        if number_assignee > numbers_without_assignee[0]:
            continue
        else:
            return number_assignee

def get_user_position(login, user_list):
    try:
        position = user_list.index(login)
        return position
    except ValueError:
        print(f"用户 {login} 未在 user_list 中找到")
        return None            

def assign_users_from_position(numbers_without_assignee, current_pr_login, user_list, url, headers):
    position = get_user_position(current_pr_login, user_list)
    for pr_number in numbers_without_assignee:      
        if current_pr_login is not None:
            position = (position + 1) % len(user_list)
            next_user = user_list[position]
            assign_user_to_pr(url, headers, pr_number, next_user)
        else:
            print(f"无法获取当前Assign: {current_pr_login} 的人员位置信息") 
 

def split_list_by_separator(input_list):
    sublists = []
    current_sublist = []
    
    for item in input_list:
        current_sublist.append(item)
        if item.get("active_lock_reason") is None:
            sublists.append(current_sublist)
            current_sublist = []
    
    if current_sublist:
        sublists.append(current_sublist)

    return sublists

def assign_user_to_pr(vcpkg_url, headers, pr_number, assignee_login):
    try: 
         result = subprocess.run(["gh", "--version"], capture_output=True, text=True)
         if result.returncode == 0:
             command = f"gh pr edit {pr_number} --add-assignee {assignee_login} --repo {vcpkg_url}"
             result = subprocess.run(command, shell=True, capture_output=True, text=True)
             if result.returncode == 0:
                 print(f"Assigned {assignee_login} to PR {pr_number}")
             else:
                 print(f"Failed to assign {assignee_login} to PR {pr_number}. Status code: {response.stderr}")
    except FileNotFoundError:
        print("GitHub CLI is not installed.")      
        
    """ 
    使用GitHub API REST:
    assignees_url = f"{url}/{pr_number}/assignees"
    data = {"assignees": [assignee_login]}
    response = requests.post(assignees_url, headers=headers, json=data)

    if response.status_code == 201:
        print(f"Assigned {assignee_login} to PR {pr_number}")
    else:
        print(f"Failed to assign {assignee_login} to PR {pr_number}. Status code: {response.status_code}")
    """
    
def main():
    
    url = "https://api.github.com/repos/microsoft/vcpkg/pulls"
    vcpkg_repo = "Microsoft/vcpkg"
    headers = {
        'User-Agent': 'Mozilla/5.0',
        'Authorization': 'token ',
        'Accept': 'application/json'
    }

    user_list_org = ["LilyWangLL","Cheney-W","FrankXie05","jimwang118","JonLiu1993","MonicaLiu0311"] 
    CTI_User_list = ["LilyWangLL","Cheney-W","FrankXie05","jimwang118","JonLiu1993","MonicaLiu0311"] 
    print("首次使用需要你本地配置GitHub CLI, 否则无法完成Assign操作！！！！！！！")
    
    json_file_path = 'assignment_log.json'

    with open(json_file_path, 'r') as json_file:
        data = json.load(json_file)

    try:
        pr_count = data['pr_count']
    except KeyError:
        pr_count = 20  

    try:
        user_to_delete_str = data['user_to_delete_str']
    except KeyError:
        user_to_delete_str = None  

  
    if user_to_delete_str:
        user_to_delete_list = [user.strip() for user in user_to_delete_str.split(',') if user.strip()]
        print(f"请假人员： {user_to_delete_str}")
            
        if user_to_delete_list is not None:
            for user_to_delete in user_to_delete_list:
                if user_to_delete in user_list_org:
                    user_list = user_list_org[:]
                    user_list.remove(user_to_delete)
                    print(f"已删除请假人员 {user_to_delete}")
                else:
                    print(f"无效的用户 {user_to_delete}， 请检查表格数据")
        else:
            print(f"请假列表是空，请检查")
    else:
        user_list = user_list_org
        print(f"表格 assignment_log.xlsx 中 user_to_delete 列为空，默认值将被使用")               
        
    print(user_list)
    all_prs_list = get_prs(url, headers, count = pr_count)
    
    all_sublists = new_func(all_prs_list)

    if all_sublists:
        all_sublists_org = del_login_by_cti(all_sublists,CTI_User_list)
        
        if  all_sublists_org:
            numbers_with_assignee, numbers_without_assignee = split_numbers_by_assignee(all_sublists_org)  
            if numbers_with_assignee and numbers_without_assignee:
                current_pr_number = compare_numbers_loop(numbers_with_assignee, numbers_without_assignee)
                pr_info = get_pr_info(url, headers, current_pr_number)
                currect_pr_login = get_assigenn_from_pr_info(pr_info) 

                if currect_pr_login is not None:
                        assign_users_from_position(numbers_without_assignee,currect_pr_login, user_list, vcpkg_repo, headers)
                        print("运行结束！！")
                else:
                    print(f"无法获取当前PR 的assign的人员信息:PR：{current_pr_number}")
            else:
                print(f"无法获取PR的number信息！")
        else:
            print(f"无法删除CTI成员的PR！")
    else:
        print(f"无法初始化列表！！")

def new_func(prs_list):
    sublists = split_list_by_separator(prs_list)
    return sublists
                    
if __name__ == "__main__":
    main()
