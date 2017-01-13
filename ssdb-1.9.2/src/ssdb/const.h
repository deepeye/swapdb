/*
Copyright (c) 2012-2014 The SSDB Authors. All rights reserved.
Use of this source code is governed by a BSD-style license that can be
found in the LICENSE file.
*/
#ifndef SSDB_CONST_H_
#define SSDB_CONST_H_

static const int SSDB_SCORE_WIDTH		= 9;
static const int SSDB_KEY_LEN_MAX		= 255;

#define RDB_6BITLEN 0
#define RDB_14BITLEN 1
#define RDB_32BITLEN 0x80
#define RDB_64BITLEN 0x81

class DataType{
public:
	static const char SYNCLOG	= 1;

	static const char META		= 'M'; // meta key
	static const char KV		= 'k';
	static const char HASH		= 'h'; // TODO CHAECK AND DELETE ! hashmap(sorted by key)
	static const char HSIZE		= 'H'; // meta value hash
	static const char ZSIZE		= 'Z'; // meta value zset
	static const char SSIZE		= 'S'; // meta value set
	static const char LSIZE		= 'L'; // meta value list

	static const char ITEM		= 'S'; // meta value set

	static const char ZSET		= 's'; // TODO CHAECK AND DELETE !   key => score
	static const char ZSCORE	= 'z';

	static const char ESCORE	= 'T'; // expire key
	static const char EKEY   	= 'E'; // expire timestamp key
	static const char QUEUE		= 'q'; // TODO CHAECK AND DELETE !
	static const char QSIZE		= 'Q'; // TODO CHAECK AND DELETE !

	static const char DELETE	= 'D';

	static const char MIN_PREFIX = HASH;
	static const char MAX_PREFIX = ZSET;
};

class BinlogType{
public:
	static const char NOOP		= 0; //
	static const char SYNC		= 1; //命令同步
	static const char MIRROR	= 2; //镜像复制-主从
	static const char COPY		= 3; //全量拷贝-多主
	static const char CTRL		= 4; //控制命令: OUT_OF_SYNC
};

class BinlogCommand{
public:
	static const char NONE  = 0;
	static const char KSET  = 1;
	static const char KDEL  = 2;
	static const char HSET  = 3;
	static const char HDEL  = 4;
	static const char ZSET  = 5;
	static const char ZDEL  = 6;

	static const char QPUSH_BACK	= 10;
	static const char QPUSH_FRONT	= 11;
	static const char QPOP_BACK		= 12;
	static const char QPOP_FRONT	= 13;
	static const char QSET			= 14;
	static const char ESET  		= 15;
	static const char EDEL  		= 16;
	static const char SSET  		= 17;
	static const char SDEL  		= 18;
	static const char DEL_KEY		= 19;

	static const char BEGIN  = 7;
	static const char END    = 8;
};

#endif
