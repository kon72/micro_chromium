ConfigurableValueInfo = provider(
    doc = "A provider for configurable values.",
    fields = {
        "value": "The value of the configurable setting.",
    },
)

bool_value = rule(
    implementation = lambda ctx: [ConfigurableValueInfo(value = ctx.attr.value)],
    attrs = {
        "value": attr.bool(
            doc = "The value of the boolean setting.",
            default = False,
        ),
    },
    doc = "A boolean-typed configurable value.",
)
