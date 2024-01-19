[1mdiff --git a/PR.py b/PR.py[m
[1mindex 8b13789..b55c007 100644[m
[1m--- a/PR.py[m
[1m+++ b/PR.py[m
[36m@@ -1 +1,284 @@[m
 [m
[32m+[m[32mfrom ast import In[m
[32m+[m[32mfrom unittest import result[m
[32m+[m[32mfrom urllib import response[m
[32m+[m[32mimport requests[m
[32m+[m[32mimport json[m
[32m+[m[32mimport subprocess[m
[32m+[m[32mimport os[m
[32m+[m[32mimport queue[m
[32m+[m[32mimport threading[m
[32m+[m[32mimport csv[m
[32m+[m[32mimport pandas as pd[m
[32m+[m
[32m+[m[32mdef input_with_timeout(prompt, timeout, default):[m
[32m+[m[32m    result_queue = queue.Queue()[m
[32m+[m
[32m+[m[32m    def input_thread():[m
[32m+[m[32m        try:[m
[32m+[m[32m            result = input(prompt)[m
[32m+[m[32m            result_queue.put(result)[m
[32m+[m[32m        except Exception as e:[m
[32m+[m[32m            result_queue.put(default)[m
[32m+[m
[32m+[m[32m    thread = threading.Thread(target=input_thread)[m
[32m+[m[32m    thread.daemon = True[m
[32m+[m[32m    thread.start()[m
[32m+[m
[32m+[m[32m    thread.join(timeout)[m
[32m+[m
[32m+[m[32m    if result_queue.qsize() > 0:[m
[32m+[m[32m        return result_queue.get()[m
[32m+[m[32m    else:[m
[32m+[m[32m        print("使用默认值")[m
[32m+[m[32m        return default[m
[32m+[m
[32m+[m[32mdef get_open_prs(url, headers, count):[m
[32m+[m[32m    params = {"per_page": count}[m
[32m+[m[32m    try:[m
[32m+[m[32m        response = requests.get(url, headers=headers, params=params)[m
[32m+[m[32m        response.raise_for_status()[m
[32m+[m[32m        return response.json()[m[41m   [m
[32m+[m[32m    except requests.RequestException as e:[m
[32m+[m[32m        print(f"Failed to retrieve PRs. Status code: {response.status_code}")[m
[32m+[m[32m        return None[m
[32m+[m
[32m+[m[32mdef get_prs(url,headers,count):[m
[32m+[m[32m    params = {"per_page": count,"state": "all"}[m
[32m+[m[32m    try:[m
[32m+[m[32m        response = requests.get(url, headers=headers, params = params)[m
[32m+[m[32m        response.raise_for_status()[m
[32m+[m[32m        return response.json()[m
[32m+[m[32m    except requests.RequestException as e:[m
[32m+[m[32m        print(f"获取最新的 {count} PR. 错误代码：{response.status_code}")[m
[32m+[m[32m        return None[m
[32m+[m
[32m+[m[32mdef get_pr_info(url, headers, pr_number):[m
[32m+[m[32m    pr_re_info = pr_number[m
[32m+[m[32m    response = requests.get(f"{url}/{pr_re_info}", headers=headers)[m
[32m+[m[41m    [m
[32m+[m[32m    if response.status_code == 200:[m
[32m+[m[32m        pr_re_info = response.json()[m
[32m+[m[41m        [m
[32m+[m[32m        if pr_re_info.get("state") == "open":[m
[32m+[m[32m            return pr_re_info[m
[32m+[m[32m        else:[m
[32m+[m[32m            return None[m
[32m+[m[32m    elif response.status_code == 404:[m
[32m+[m[32m        print(f"pr: {pr_number} 不存在")[m
[32m+[m[32m        return None[m
[32m+[m[32m    else:[m
[32m+[m[32m        print(f" 无法找到pr: {pr_number}, 错误代码：{response.status_code}")[m
[32m+[m[32m        return None[m
[32m+[m
[32m+[m[32mdef get_assigenn_from_pr_info(pr_info):[m
[32m+[m[32m    if pr_info:[m
[32m+[m[32m        currect_pr_login = pr_info["assignee"]["login"][m
[32m+[m[32m        return currect_pr_login[m
[32m+[m[32m    else:[m
[32m+[m[32m        return None[m
[32m+[m
[32m+[m
[32m+[m[32mdef del_login_by_cti(sublist_org, CTI_User_list):[m
[32m+[m[32m    indexes_to_remove = [][m
[32m+[m
[32m+[m[32m    for i, sublist in enumerate(sublist_org):[m
[32m+[m[32m        user_info = sublist[0][m
[32m+[m[32m        login = user_info["user"]["login"][m
[32m+[m[41m        [m
[32m+[m[32m        if login in CTI_User_list:[m
[32m+[m[32m            indexes_to_remove.append(i)[m
[32m+[m
[32m+[m[32m    for index in reversed(indexes_to_remove):[m
[32m+[m[32m        del sublist_org[index][m
[32m+[m
[32m+[m[32m    return sublist_org[m
[32m+[m
[32m+[m[32mdef split_numbers_by_assignee(list_org):[m
[32m+[m[32m    numbers_with_assignee = [][m
[32m+[m[32m    numbers_without_assignee = [][m
[32m+[m
[32m+[m[32m    for list in list_org:[m
[32m+[m[32m        list_info = list[0][m
[32m+[m[32m        assignee = list_info["assignee"][m
[32m+[m[32m        number = list_info["number"][m
[32m+[m[32m        state = list_info["state"][m
[32m+[m
[32m+[m[32m        if assignee is not None:[m
[32m+[m[32m            numbers_with_assignee.append(number)[m
[32m+[m[41m            [m
[32m+[m[32m        else:[m
[32m+[m[32m            if state == 'open':[m
[32m+[m[32m                numbers_without_assignee.append(number)[m
[32m+[m[41m            [m
[32m+[m[32m        numbers_with_assignee_sorted = sorted(numbers_with_assignee, reverse=True)[m
[32m+[m[32m        numbers_without_assignee_sorted = sorted(numbers_without_assignee)[m
[32m+[m[41m    [m
[32m+[m[32m    return numbers_with_assignee_sorted, numbers_without_assignee_sorted[m
[32m+[m
[32m+[m[32mdef compare_numbers_loop(numbers_with_assignee, numbers_without_assignee):[m
[32m+[m[32m    for number_assignee in numbers_with_assignee:[m
[32m+[m[41m        [m
[32m+[m[32m        if number_assignee > numbers_without_assignee[0]:[m
[32m+[m[32m            continue[m
[32m+[m[32m        else:[m
[32m+[m[32m            return number_assignee[m
[32m+[m
[32m+[m[32mdef get_user_position(login, user_list):[m
[32m+[m[32m    try:[m
[32m+[m[32m        position = user_list.index(login)[m
[32m+[m[32m        return position[m
[32m+[m[32m    except ValueError:[m
[32m+[m[32m        print(f"用户 {login} 未在 user_list 中找到")[m
[32m+[m[32m        return None[m[41m            [m
[32m+[m
[32m+[m[32mdef assign_users_from_position(numbers_without_assignee, current_pr_login, user_list, url, headers):[m
[32m+[m[32m    position = get_user_position(current_pr_login, user_list)[m
[32m+[m[32m    for pr_number in numbers_without_assignee:[m[41m      [m
[32m+[m[32m        if current_pr_login is not None:[m
[32m+[m[32m            position = (position + 1) % len(user_list)[m
[32m+[m[32m            next_user = user_list[position][m
[32m+[m[32m            assign_user_to_pr(url, headers, pr_number, next_user)[m
[32m+[m[32m        else:[m
[32m+[m[32m            print(f"无法获取当前Assign: {current_pr_login} 的人员位置信息")[m[41m [m
[32m+[m[41m [m
[32m+[m
[32m+[m[32mdef split_list_by_separator(input_list):[m
[32m+[m[32m    sublists = [][m
[32m+[m[32m    current_sublist = [][m
[32m+[m[41m    [m
[32m+[m[32m    for item in input_list:[m
[32m+[m[32m        current_sublist.append(item)[m
[32m+[m[32m        if item.get("active_lock_reason") is None:[m
[32m+[m[32m            sublists.append(current_sublist)[m
[32m+[m[32m            current_sublist = [][m
[32m+[m[41m    [m
[32m+[m[32m    if current_sublist:[m
[32m+[m[32m        sublists.append(current_sublist)[m
[32m+[m
[32m+[m[32m    return sublists[m
[32m+[m
[32m+[m[32mdef assign_user_to_pr(vcpkg_url, headers, pr_number, assignee_login):[m
[32m+[m[32m    try:[m[41m [m
[32m+[m[32m         result = subprocess.run(["gh", "--version"], capture_output=True, text=True)[m
[32m+[m[32m         if result.returncode == 0:[m
[32m+[m[32m             command = f"gh pr edit {pr_number} --add-assignee {assignee_login} --repo {vcpkg_url}"[m
[32m+[m[32m             result = subprocess.run(command, shell=True, capture_output=True, text=True)[m
[32m+[m[32m             if result.returncode == 0:[m
[32m+[m[32m                 print(f"Assigned {assignee_login} to PR {pr_number}")[m
[32m+[m[32m             else:[m
[32m+[m[32m                 print(f"Failed to assign {assignee_login} to PR {pr_number}. Status code: {response.stderr}")[m
[32m+[m[32m    except FileNotFoundError:[m
[32m+[m[32m        print("GitHub CLI is not installed.")[m[41m      [m
[32m+[m[41m        [m
[32m+[m[32m    """[m[41m [m
[32m+[m[32m    使用GitHub API REST:[m
[32m+[m[32m    assignees_url = f"{url}/{pr_number}/assignees"[m
[32m+[m[32m    data = {"assignees": [assignee_login]}[m
[32m+[m[32m    response = requests.post(assignees_url, headers=headers, json=data)[m
[32m+[m
[32m+[m[32m    if response.status_code == 201:[m
[32m+[m[32m        print(f"Assigned {assignee_login} to PR {pr_number}")[m
[32m+[m[32m    else:[m
[32m+[m[32m        print(f"Failed to assign {assignee_login} to PR {pr_number}. Status code: {response.status_code}")[m
[32m+[m[32m    """[m
[32m+[m[41m    [m
[32m+[m[32mdef main():[m
[32m+[m[41m    [m
[32m+[m[32m    url = "https://api.github.com/repos/microsoft/vcpkg/pulls"[m
[32m+[m[32m    vcpkg_repo = "Microsoft/vcpkg"[m
[32m+[m[32m    headers = {[m
[32m+[m[32m        'User-Agent': 'Mozilla/5.0',[m
[32m+[m[32m        'Authorization': 'token ',[m
[32m+[m[32m        'Accept': 'application/json'[m
[32m+[m[32m    }[m
[32m+[m
[32m+[m[32m    user_list_org = ["LilyWangLL","Cheney-W","FrankXie05","jimwang118","JonLiu1993","MonicaLiu0311"][m[41m [m
[32m+[m[32m    CTI_User_list = ["LilyWangLL","Cheney-W","FrankXie05","jimwang118","JonLiu1993","MonicaLiu0311"][m[41m [m
[32m+[m[32m    print("首次使用需要你本地配置GitHub CLI, 否则无法完成Assign操作！！！！！！！")[m
[32m+[m[41m    [m
[32m+[m[32m    file_path = 'assignment_log.xlsx'[m
[32m+[m[32m    default_pr_count = 20[m
[32m+[m[41m    [m
[32m+[m[32m    df = pd.read_excel(file_path)[m
[32m+[m[32m    try:[m
[32m+[m[32m        pr_count_str = df.iloc[0, 0][m
[32m+[m[32m        pr_count = pr_count_str[m
[32m+[m[32m    except ValueError:[m
[32m+[m[32m        pr_count_str = False[m
[32m+[m[32m        pr_count = default_pr_count[m
[32m+[m[32m    except FileNotFoundError:[m
[32m+[m[32m        pr_count_str = False[m
[32m+[m[32m        pr_count = default_pr_count[m
[32m+[m[32m    except Exception as e:[m
[32m+[m[32m        pr_count_str = False[m
[32m+[m[32m        pr_count = default_pr_count[m
[32m+[m[41m    [m
[32m+[m[32m    if pr_count_str:[m
[32m+[m[32m        pr_count = pr_count_str[m
[32m+[m[32m    else:[m
[32m+[m[32m        print(f"表格 assignment_log.xlsx 中 pr_count 列为空，默认值将被使用")[m[41m  [m
[32m+[m
[32m+[m[32m    try:[m
[32m+[m[32m        user_to_delete_str = df.iloc[0, 1][m
[32m+[m[32m        pr_count = pr_count_str[m
[32m+[m[32m    except ValueError:[m
[32m+[m[32m        user_to_delete_str = False[m
[32m+[m[32m        user_list = user_list_org[m
[32m+[m[32m    except FileNotFoundError:[m
[32m+[m[32m        user_to_delete_str = False[m
[32m+[m[32m        user_list = user_list_org[m
[32m+[m[32m    except Exception as e:[m
[32m+[m[32m        user_to_delete_str = False[m
[32m+[m[32m        user_list = user_list_org[m
[32m+[m[41m  [m
[32m+[m[32m    if user_to_delete_str:[m
[32m+[m[32m        user_to_delete_list = [user.strip() for user in user_to_delete_str.split(',') if user.strip()][m
[32m+[m[32m        print(f"请假人员： {user_to_delete_str}")[m
[32m+[m[41m            [m
[32m+[m[32m        if user_to_delete_list is not None:[m
[32m+[m[32m            for user_to_delete in user_to_delete_list:[m
[32m+[m[32m                if user_to_delete in user_list_org:[m
[32m+[m[32m                    user_list = user_list_org[:][m
[32m+[m[32m                    user_list.remove(user_to_delete)[m
[32m+[m[32m                    print(f"已删除请假人员 {user_to_delete}")[m
[32m+[m[32m                else:[m
[32m+[m[32m                    print(f"无效的用户 {user_to_delete}， 请检查表格数据")[m
[32m+[m[32m        else:[m
[32m+[m[32m            print(f"请假列表是空，请检查")[m
[32m+[m[32m    else:[m
[32m+[m[32m        print(f"表格 assignment_log.xlsx 中 user_to_delete 列为空，默认值将被使用")[m[41m               [m
[32m+[m[41m        [m
[32m+[m[32m    print(user_list)[m
[32m+[m[32m    all_prs_list = get_prs(url, headers, count = pr_count)[m
[32m+[m[41m    [m
[32m+[m[32m    all_sublists = new_func(all_prs_list)[m
[32m+[m
[32m+[m[32m    if all_sublists:[m
[32m+[m[32m        all_sublists_org = del_login_by_cti(all_sublists,CTI_User_list)[m
[32m+[m[41m        [m
[32m+[m[32m        if  all_sublists_org:[m
[32m+[m[32m            numbers_with_assignee, numbers_without_assignee = split_numbers_by_assignee(all_sublists_org)[m[41m  [m
[32m+[m[32m            if numbers_with_assignee and numbers_without_assignee:[m
[32m+[m[32m                current_pr_number = compare_numbers_loop(numbers_with_assignee, numbers_without_assignee)[m
[32m+[m[32m                pr_info = get_pr_info(url, headers, current_pr_number)[m
[32m+[m[32m                currect_pr_login = get_assigenn_from_pr_info(pr_info)[m[41m [m
[32m+[m
[32m+[m[32m                if currect_pr_login is not None:[m
[32m+[m[32m                        assign_users_from_position(numbers_without_assignee,currect_pr_login, user_list, vcpkg_repo, headers)[m
[32m+[m[32m                        print("运行结束！！")[m
[32m+[m[32m                else:[m
[32m+[m[32m                    print(f"无法获取当前PR 的assign的人员信息:PR：{current_pr_number}")[m
[32m+[m[32m            else:[m
[32m+[m[32m                print(f"无法获取PR的number信息！")[m
[32m+[m[32m        else:[m
[32m+[m[32m            print(f"无法删除CTI成员的PR！")[m
[32m+[m[32m    else:[m
[32m+[m[32m        print(f"无法初始化列表！！")[m
[32m+[m
[32m+[m[32mdef new_func(prs_list):[m
[32m+[m[32m    sublists = split_list_by_separator(prs_list)[m
[32m+[m[32m    return sublists[m
[32m+[m[41m                    [m
[32m+[m[32mif __name__ == "__main__":[m
[32m+[m[32m    main()[m
