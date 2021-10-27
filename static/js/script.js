$(function() {
    $(".forgotBtn").click(function() { $("#forgot").toggleClass("toggle") });
    $(".registerBtn").click(function() { $("#register, #formContainer").toggleClass("toggle") })
});

function check_reg() {
    let uer = document.getElementById("user").value;
    let pwd = document.getElementById("passwd").value;
    let re_pwd = document.getElementById("re_passwd").value;
    let reg = /^\s*$/;
    if (reg.test(user) || reg.test(passwd) || reg.test(re_pwd)) {
        alert("用户名或密码不能为空");
        return false;
    }
    if (pwd == re_pwd) {
        return true;
    } else {
        alert("两次输入密码不一致");
        return false;
    }
}

function check_login() {
    let user = document.getElementById("l_user").value;
    let passwd = document.getElementById("l_passwd").value;
    let reg = /^\s*$/;
    if (reg.test(user) || reg.test(passwd)) {
        alert("用户名或密码不能为空");
        return false;
    } else {
        return true;
    }
}


function delete_(data) {
    var parentTr = data.parentNode.parentNode.parentNode.parentNode;
    var node_del = parentTr.parentNode;
    var table = node_del.parentNode;
    var id = parentTr.children[0].innerText;
    var user = parentTr.children[1].innerText;
    var info = {
        "id": id,
        "user": user
    }
    $.ajax({
        type: 'POST',
        url: '/delete',
        data: JSON.stringify(info),
        contentType: 'application/json; charset=UTF-8',
        dataType: 'json',
        success: function(data) {},
        error: function(xhr, type) { alert("error") }
    });
}

function edit(data) {
    var parentTr = data.parentNode.parentNode.parentNode.parentNode;
    var id = parentTr.children[0].innerText;
    document.getElementById('doc-new-name-1').value = id;
    toggleFade('edit');
    event.preventDefault();
}

function newCard() {
    var doc = window.parent.document;
    var id = doc.querySelector("body > div.jq_dvpanel > div:nth-child(1) > div").innerText;
    var data = {
        "id": id,
        "opt": "new"
    }
    $.ajax({
        type: 'POST',
        url: '/card_about',
        data: JSON.stringify(data),
        contentType: 'application/json; charset=UTF-8',
        dataType: 'json',
        success: function(data) {
            window.parent.location.reload();
            alert("新建卡成功")
        },
        error: function(xhr, type) { alert("error") }
    });
}


function card(data) {
    var formData = new FormData(data);
    var id = formData.get('card_id');
    var money = formData.get('amount');
    var a_type = "sub";
    if (data.getElementsByClassName('add').length) {
        a_type = "add";
    }
    var info = {
        "id": id,
        "amount": money,
        "type": a_type
    }
    $.ajax({
        type: 'POST',
        url: '/money_about',
        //url: '',
        data: JSON.stringify(info),
        contentType: 'application/json; charset=UTF-8',
        dataType: 'json',
        success: function(data) {
            window.location.reload();
            console.log(data["msg"].contentType)
            alert(data["msg"])
        },
        error: function(xhr, type) { alert("error") }
    });
};