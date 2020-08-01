# Application.mk

# コマンドライン許容文字数対策
APP_SHORT_COMMANDS := true

# options
APP_CPPFLAGS :=	-O3 \
				-Wall \
				-fno-exceptions \
				-fno-rtti
