var mongodb = require('mongodb');
console.log("typeof mongodb=",typeof(mongodb));
var server = new mongodb.Server('localhost',27017,{auto_reconnect:true});
var db = new mongodb.Db('mydb',server,{safe:true});
db.open(function(err,db){
    if(!err){   
        db.collection('mycoll',{safe:true},function(err,collection){
            var tmp1 = {title:'hello',number:1};
            collection.insert(tmp1,{safe:true},function(err,result){
                if(err){
                    console.log("db insert err=",err);
                    return;
                }
                console.log("db insert ok,res=",result);
            }); 
        });
    }else{
        console.log("db open err=",err);
    }   
});
