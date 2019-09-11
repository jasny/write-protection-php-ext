--TEST--
Test readonly example from README
--FILE--
<?php

class Foo extends WriteProtection
{
    /**
     * @readonly
     */
    public int $bar = 0;

    protected function isReadOnlyProperty(string $name): bool
    {
        $doc = (new ReflectionProperty(get_class($this), $name))->getDocComment();
        return strpos($doc, "@readonly") !== false;
    }

    public function setPublicProperty(string $name, $value): void
    {
        if ($this->isReadOnlyProperty($name)) {
            echo "Unable to set read-only property Foo::{$name}\n";
            return;
        }

        $this->{$name} = $value;
    }

    public function unsetPublicProperty(string $name): void
    {
        if ($this->isReadOnlyProperty($name)) {
            echo "Unable to unset read-only property Foo::{$name}\n";
            return;
        }

        unset($this->{$name});
    }
}

$foo = new Foo();

$foo->bar = 100;
unset($foo->bar);

?>
--EXPECT--
Unable to set read-only property Foo::bar
Unable to unset read-only property Foo::bar
