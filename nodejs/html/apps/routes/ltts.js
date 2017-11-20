var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  if(req.headers['user-agent'].match(/(iPhone|iPod|Android|ios)/i)){
    res.render('ltts/mltts', { title: 'Express' });
  }else{
    res.render('ltts/ltts', { title: 'Express' });
  }
});

module.exports = router;
