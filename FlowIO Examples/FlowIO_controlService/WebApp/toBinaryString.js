//We are extending the Number class to display leading zeros
//gist.github.com/endel/321925f6cafa25bbfbde
Number.prototype.toBinaryString = function(size) {
  var s = this.toString(2);
  while (s.length < size) {
    s = "0" + s;
  }
  return s;
}