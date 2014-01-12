var FFT = function(n) {
    this.n = n;
    this.BITREV = new Array(n+1);
    this.SINTBL = new Float32Array(n+1);
    this._init();
};

FFT.prototype = {

    _init : function() {
	var i = 0;
	var j = 0;
	var n2 = this.n >> 1;
	for (;;) {
	    this.BITREV[i] = j;
	    if (++i >= this.n)
		break;
	    var k = n2;
	    while (k <= j) {
		j -= k;
		k >>= 1;
	    }
	    j += k;
	}
	n2 = this.n >> 1;
	var n4 = this.n >> 2;
	var n8 = this.n >> 3;
	var t = Math.sin(Math.PI / this.n);
	var dc = 2 * t * t;
	var ds = Math.sqrt(dc * (2 - dc));
	t = 2 * dc;
	var c = this.SINTBL[n4] = 1;
	var s = this.SINTBL[0] = 0;
	for (var i = 1; i < n8; i++) {
	    c -= dc;
	    dc += t * c;
	    s += ds;
	    ds -= t * s;
	    this.SINTBL[i] = s;
	    this.SINTBL[n4 - i] = c;
	}
	if (n8 != 0) {
	    this.SINTBL[n8] = Math.sqrt(0.5);
	}
	for (var i = 0; i < n4; i++) {
	    this.SINTBL[n2 - i] = this.SINTBL[i];
	}
	for (var i = 0; i < n2 + n4; i++) {
	    this.SINTBL[i + n2] = - this.SINTBL[i];
	}
    },

    fft : function(x, y) {
	var n4 = this.n >> 2;
	// ビット反転
	for (var i = 0; i < this.n; i++) {
	    var j = this.BITREV[i];
	    if (i < j) {
		var t = x[i]; x[i] = x[j]; x[j] = t;
		t = y[i]; y[i] = y[j]; y[j] = t;
	    }
	}
	// 変換
	var k2 = 0;
	var d = this.n;
	for (var k = 1; k < this.n; k = k2) {
	    var h = 0;
	    k2 = k << 1;
	    d <<= 1;
	    for (var j = 0; j < k; j++) {
		var c = this.SINTBL[h + n4];
		var s = this.SINTBL[h];
		for (var i = j; i < this.n; i += k2) {
		    var ik = i + k;
		    var dx = s * y[ik] + c * x[ik];
		    var dy = c * y[ik] - s * x[ik];
		    x[ik] = x[i] - dx;
		    x[i] += dx;
		    y[ik] = y[i] - dy;
		    y[i] += dy;
		}
		h += d;
	    }
	}
	for (var i = 0; i < this.n; i++) {
	    x[i] /= n;
	    y[i] /= n;
	}
    },

    ifft : function(x, y) {
	var n4 = this.n >> 2;
	// ビット反転
	for (var i = 0; i < this.n; i++) {
	    var j = this.BITREV[i];
	    if (i < j) {
		var t = x[i]; x[i] = x[j]; x[j] = t;
		t = y[i]; y[i] = y[j]; y[j] = t;
	    }
	}
	// 変換
	var k2 = 0;
	var d = this.n;
	for (var k = 1; k < this.n; k = k2) {
	    var h = 0;
	    k2 = k << 1;
	    d <<= 1;
	    for (var j = 0; j < k; j++) {
		var c = this.SINTBL[h + n4];
		var s = -this.SINTBL[h];
		for (var i = j; i < this.n; i += k2) {
		    var ik = i + k;
		    var dx = s * y[ik] + c * x[ik];
		    var dy = c * y[ik] - s * x[ik];
		    x[ik] = x[i] - dx;
		    x[i] += dx;
		    y[ik] = y[i] - dy;
		    y[i] += dy;
		}
		h += d;
	    }
	}
    },

    abs : function(x, y) {
	var a = new Float32Array(n);
	for (var i = 0; i < this.n; i++) {
	    a[i] = Math.sqrt(x[i]*x[i] + y[i]*y[i]);
	}
	return a;
    },

    angle : function(x, y) {
	var a = new Float32Array(n);
	for (var i = 0; i < this.n; i++) {
	    a[i] = Math.atan2(y[i], x[i]);
	}
	return a;
    },

    hamming : function(x) {
	// 0.54 - 0.46 * cos(2pi * x), if 0 <= x <= 1
	for (var i = 0; i < this.n; i++) {
	    var t = i / this.n;
	    x[i] *= 0.54 - 0.46 * Math.cos(2 * Math.PI * t);
	}
    },

    hanning : function(x) {
	// 0.5 - 0.5 * cos(2pi * x), if 0 <= x <= 1
	for (var i = 0; i < this.n; i++) {
	    var t = i / this.n;
	    x[i] *= 0.5 - 0.5 * Math.cos(2 * Math.PI * t);
	}
    },

};
