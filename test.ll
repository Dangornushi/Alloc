define i32 @main() #0 {
entry:
	
	%0 = alloca i32*, align 8
	%1 = alloca i32, align 4
	store i32* %1, i32** %0, align 8
	store i32 12, i32* %1, align 4

	ret i32 %1
}
