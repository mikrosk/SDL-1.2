#!/usr/bin/awk -f

BEGIN {
	inif = 0;
	inelse = 0;
	funcno = 0;
	ifstartfunc = 0;
	ifdef = "";
	nexports = "nexports.h"
	print "/* automatically generated --- DO NOT EDIT --- */" > nexports;
}

/^#if/ || /^#ifndef/ {
	if (inif != 0) {
		print "cannot handle nested #if" > "/dev/stderr";
		exit(1);
	}
	inif++;
	inelse = 0;
	ifdef = $0;
	ifstartfunc = funcno;
	print $0 > nexports;
	next;
}

/^#else/ {
	if (inif == 0) {
		print "#else without #if" > "/dev/stderr";
		exit(1);
	}
	inelse = 1;
	print $0 > nexports;
	next;
}

/^#endif/ {
	if (inif == 0) {
		print "#endif without #if" > "/dev/stderr";
		exit(1);
	}
	inif--;
	if (inelse != 0 && ifstartfunc != funcno) {
		print "nonmatching NOFUNCs" > "/dev/stderr";
		exit(1);
	}
	inelse = 0;
	ifdef = "";
	print $0 > nexports;
	next;
}

/^#define/ {
		print $0 > nexports;
		next;
	   }

/^#undef/ {
		print $0 > nexports;
		next;
	  }

/^#error/ {
		print $0 > nexports;
		next;
	  }

/^#/	{
		printf("%d: unrecognized preprocessor: %s\n", NR, $0) > "/dev/stderr";
		exit(1);
	}

{
	if (match($0, /^[^#](.*)(LIBFUNCRET64)\((.*)$/, a)) {
		file = "imp_" funcno ".c";
		print "#include \"lib_imp.h\"" > file;
		if (inif == 1)
			print ifdef > file;
		macro = funcno >= 128 ? "LIBFUNC2" : "LIBFUNC";
		printf("\t/* %4d */ %sRET64(%d, %s\n", funcno, macro, funcno, a[3]) > file;
		if (inif == 1)
			print "#endif" > file;
		close(file);
		printf("\t/* %4d */ %sRET64(%d, %s\n", funcno, macro, funcno, a[3]) > nexports;
		++funcno;
	} else if (match($0, /^[^#](.*)(LIBFUNC)\((.*)$/, a)) {
		file = "imp_" funcno ".c";
		print "#include \"lib_imp.h\"" > file;
		if (inif == 1)
			print ifdef > file;
		macro = funcno >= 128 ? "LIBFUNC2" : "LIBFUNC";
		printf("\t/* %4d */ %s(%d, %s\n", funcno, macro, funcno, a[3]) > file;
		if (inif == 1)
			print "#endif" > file;
		close(file);
		printf("\t/* %4d */ %s(%d, %s\n", funcno, macro, funcno, a[3]) > nexports;
		++funcno;
	} else if (match($0, /^(.*)(NOFUNC)(.*)$/, a) && inif == 0 && $1 != "#undef") {
		file = "imp_" funcno ".c";
		print "#include \"lib_imp.h\"" > file;
		close(file);
		printf("\t/* %4d */ %s\n", funcno, a[2]) > nexports;
		++funcno;
	} else if (match($0, /^(.*)(NOFUNC)(.*)$/, a) && inelse != 0) {
		printf("\t/* %4d */ %s\n", ifstartfunc, a[2]) > nexports;
		++ifstartfunc;
	} else {
		print $0 > nexports;
	}
}
