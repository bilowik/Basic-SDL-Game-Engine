G_REL_CURR_DIR:=$(G_REL_CURR_DIR)/$(shell basename $$PWD)


sub-dir-default:
	$(MAKE) -C test
	$(MAKE) -C osGame
	$(MAKE) -C sprites

sub-dir-clean:
	$(MAKE) -C test clean
	$(MAKE) -C osGame clean
	$(MAKE) -C sprites clean

clean: sub-dir-clean

print-%: ;@echo $* = $($*)
